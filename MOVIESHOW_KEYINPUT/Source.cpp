#include "ms_exp_showmovie.hpp"
#include "msexp_misc.hpp"
#include "msexp_eyetribe.hpp"
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <ctime>
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//デバッグフラグ これがONだと75％の大きさで窓作成
#define		DEBUGMODE	false

//WindowClass名の設定
#define		classNAME	_T("MYWINDOWDDD")
//背景色とか，そんなもののDeclare
#define		BGMAIN	RGB(127,127,127)
//実験のくりかえし数
#define		MOVIENUM	7
//動画ソースの格納スペース
#define		MOVIESOURCENUM 8
//ATKカウント
#define     DELAYCOUNT     1
//初期化適当な値
#define		INITIALIZED   5000.0
//MovShotcut
#define		MOVS 5



//画面全体を隠すWindowの作成と，キー入力などによるInteractionの実装
//基本方針としては，Windowsの窓周り関数をそのまま利用F
//InterationもOSに依存する
// HINSTANCEやら HWNDやらは，とりあえずおまじないと言うことでw

HWND			thewnd;
int				movcount;		//実験動画の現在セクション
SYSTEMTIME		timenow;
FILE			*fp;
errno_t			err;
bool			flgStop;
bool			isDispose;		//動画終了初回のみ通る処理用フラグ。再生時はTrueに戻す。再生終了の処理が終わったらFalseに切り替える
bool			isPause;		//今止まってますか！？？？？
bool			isPlay;			//いま再生してますか
BSTR			MOVFILEPATH[MOVIESOURCENUM];//動画の場所

int unit, cnt, cnt2;
DOUBLE tempN;
bool flugC;

//動画ごとET記録ファイル命名規則
std::string st;
SYSTEMTIME tm;
char timestr[20];
std::string exID = "sub02";

//ある動画中でどのタイミングでどういう長さの遅延が起こるかを定義したもの
//今入っているのは初期化
//動画が切り替わるたびに、1行目をRondomdelay()で、2行目をDelaytimeset()で切り替えていく
DOUBLE		delayProgram[2][DELAYCOUNT+1] = {	{6 ,/*1.0, 1.5 ,2.0	,2.5,*/ 100.0 },
										{ INITIALIZED,/*INITIALIZED,INITIALIZED,INITIALIZED,INITIALIZED,*/999.9 }};//遅延のスクリプト

DOUBLE		delayTiming[DELAYCOUNT];
DOUBLE		delayLength[7] = {0,0,0,500,1000,2000,5000};
//DOUBLE		delayLength[7] = { 0,0,0,0,0,0,0 };

std::string movname_a;

//動画再生の順番設定
//FILEPATHは動画ごとの通し番号、MOVIEORDERは順番を示す。
//MOVIEORDER[2]=7は、2番めにMOVFILEPATH[7]を再生せよということ
int	MOVIEORDER[MOVIENUM] = { MOVS,MOVS,MOVS,MOVS,MOVS,MOVS,MOVS};
int excount = 1;

////インスタンス作成
MSEXP::ShowMov	mov;//例の追加ヘッダ
MSEXP::EYETRIBE tet;//アイトライブ

//乱数
std::random_device rnd;
std::mt19937 mt(rnd());
std::uniform_int_distribution<> rand29(20, 40);//乱数の範囲ここで決まってます。動画開始から5~15秒

void RandomDelay() {

	for (cnt = 0; cnt < DELAYCOUNT; cnt++) {

		do {
			flugC = true;
			tempN = rand29(mt);
			for (int cnt2 = 0; cnt2 < cnt; cnt2++) { 
				if (delayTiming[cnt2] == tempN) {flugC = false; } }//もし重複があれば脱出させない
		} while (flugC == false);

		delayTiming[cnt] = tempN;
	}

	sort(delayTiming, delayTiming + DELAYCOUNT);

	for (int i = 0; i < DELAYCOUNT; i++) {
		delayProgram[0][i] = delayTiming[i]/2;
	}
	
}

void DelayLengthSet(int moviecount ){
	for (int i = 0; i < DELAYCOUNT; i++) {
		delayProgram[1][i] = delayLength[moviecount];
		}
}

void ShuffleDelayLength(double ary[], int size)
{
	for (int i = 0; i<size; i++)
	{
		int j = rand() % size;
		double t = ary[i];
		ary[i] = ary[j];
		ary[j] = t;
	}
}

//ここからメイン文が走る
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmdline, int cmdshnow) {
	MSG			msg;
	HBRUSH		hbrsh;
	WNDCLASSEX	wcex;
	int			dispx, dispy;

	//プログラム開始時のお約束。音が鳴る。時刻を基に乱数初期化する。
	Beep(577, 100);
	Beep(577, 100);
	srand((unsigned)time(NULL));
	
	//初期化
	movcount = 0;
	unit = 0;
	isDispose = true;
	isPause = false;
	isPlay = false;

	for (int i; i < DELAYCOUNT; i++) { delayTiming[i] = 0.0; }

	GetSystemTime(&tm);
	sprintf_s(timestr, "%02d%02d_%02d%02d_",tm.wMonth,tm.wDay,(tm.wHour+9)%24,tm.wMinute);

	//遅延帳並び替え
	ShuffleDelayLength(delayLength, 7);
	//ランダム
	//Delayタイミングの作成
	RandomDelay();

	//ファイル名
	struct tm now;
	time_t longtime;
	//char timestr[32];
		//longtime = time(NULL);
	//localtime_s(&now, &longtime);
	//asctime_s(timestr, sizeof(timestr), &now);
	


	//動画ソース管理。基本的には動画追加時以外触らないで、再生順はMOVIEORDERで入れ替える
	MOVFILEPATH[0] = SysAllocString(L"../mov/Wildlife.wmv");
	MOVFILEPATH[1] = SysAllocString(L"../mov/cookie.avi");
	MOVFILEPATH[2] = SysAllocString(L"../mov/fix_kmn20m.wmv");
	MOVFILEPATH[3] = SysAllocString(L"../mov/fix_ten20m.wmv");
	MOVFILEPATH[4] = SysAllocString(L"../mov/fix_ter20m.wmv");
	MOVFILEPATH[5] = SysAllocString(L"../mov/30_itorinos20m.wmv");
	MOVFILEPATH[6] = SysAllocString(L"../mov/30_kimono20m.wmv");
	MOVFILEPATH[7] = SysAllocString(L"../mov/30_terras20m.wmv");
	//MOVFILEPATH[8] = SysAllocString(L"");
	//MOVFILEPATH[9] = SysAllocString(L"");



	//EYETRIBE setup
	tet.StartListening("TEST1");
	Sleep(2000);
	tet.Setparam_i1(0);


	//画面サイズの取得　DEBUGMODEがtrueの場合は，縦横半分で作る
	dispx = GetSystemMetrics(SM_CXSCREEN);
	dispy = GetSystemMetrics(SM_CYSCREEN);
	if (DEBUGMODE) {
		dispx = dispx*.75;
		dispy = dispy*.75;
	}

	//親ウィンドウの作成
	//背景塗りようのブラシの作成
	hbrsh = CreateSolidBrush(BGMAIN);

	//akisamo
	//タイムスタンプfileの出力
	char filename[100];
	sprintf_s(filename,  "./data/%s_%s.txt"  , exID, std::string( timestr, 9));

	err = fopen_s(&fp, filename, "w");
	if (err == 0) {//開けなかったら
		printf("open\n");
	}
	else {
		printf("error\n");
		return 1;
	}

	//CreateWindowExが参照する窓周りの諸々の設定。俺もよくわからん。
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinst;
	wcex.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = hbrsh;//背景塗ってる
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = classNAME;//窓作成の際に参照されるっぽい。
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	RegisterClassEx(&wcex);

	//窓作成
	thewnd = CreateWindowEx(NULL, classNAME, classNAME, WS_POPUP, 0, 0, dispx, dispy, NULL, NULL, hinst, NULL);
	if (thewnd == NULL) {
		return 0;
	}

	//窓に対してたいまー関数の設定
	SetTimer(thewnd, 20, 20, NULL);

	//作成した窓を表示（これを呼ばなくても表示されてはいるはず）
	ShowWindow(thewnd, cmdshnow);

	//マウスカーソル消去（ただし，動画領域は別ウィンドウなのでこの設定は無効になる）
	ShowCursor(FALSE);

	//まじないなむなむ
	UpdateWindow(thewnd);

	// DIRECTSHOW setup



	//以下のWhileループが機能している間，ウィンドウが表示されている。
	// その実態は，OSからのイベントに応じて呼び出される  CALLBACK WndProc()が回っている
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ShowCursor(true);

	UnregisterClass(classNAME, hinst);
	tet.EndListening();
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	//この中身は，OSからのCallによって「その都度」呼び出される。
	//従って，Scopeがこの関数の中だけで終わるものは，その都度初期化される。

	//akisamo
	HDC         hdc, hMdc, hdcBmp;
	PAINTSTRUCT ps;
	HBITMAP     hbmp;
	LPTSTR		stms = TEXT("うりゃ");
	HBRUSH		br;


	switch (uMsg) {

	case WM_PAINT://再描画処理


		if (isDispose) {//dispose
			//変える
			RandomDelay();
			//計測の終了、fileを開き直して計測スタート
			tet.Setparam_s1("Dispose");
			tet.EndListening();
			Sleep(1000);
			unit = 0;
			//movname_a = std::to_string(movcount);
			//tet.StartListening(movname_a);
			//Sleep(2000);
			//tet.Setparam_i1(0);

			//画面消しと何かあれ
			Beep(750, 100);
			hdc = BeginPaint(hwnd, &ps);
			SetBkColor(hdc, RGB(128, 128, 128));
			SetTextColor(hdc, RGB(0, 0, 0));
			int lineH = 40;
			int Fline = 20;
			//TextOut(hdc, 100, Fline + lineH, stms, strlen(stms));
			EndPaint(hwnd, &ps);
		}

		break;

	case WM_TIMER:
		if(isPlay == true){
			tet.Setparam_s1("playing");
			tet.Setparam_d1(mov.GetCurrentPosition());
		}
		else {
			tet.Setparam_d1(0);
		}
		//再生終了した初回のみ動画画面を閉じる処理を行う
		if (!mov.isMoviePlaying()) {
			if (!isDispose) {//初回のみとおる判定。再生始まったらフラグを切り替える
				mov.DisposeMovieScreen();
				InvalidateRect(hwnd, NULL, TRUE);
				isDispose = true;
				isPlay = false;
				tet.Setparam_s1("Dispose");
			}
		//再生中の擬似的なDelayを行う
		} else if (delayProgram[0][unit] < mov.GetCurrentPosition()) {	
				tet.Setparam_s1("Delay_"+ std::to_string(delayProgram[0][0]));
				tet.Setparam_i3(6);
				tet.Setparam_d2(delayProgram[1][unit]);
				mov.StopMovie();
				Sleep(delayProgram[1][unit]);
				mov.StartMovie();
				tet.Setparam_s1("ReStart");
				tet.Setparam_i3(0);
				tet.Setparam_d2(0);
				unit = unit+1;
					}
		break;

	case WM_KEYUP:
		if(wParam == VK_SPACE){
								tet.Setparam_i1(0);
		}
		break;

	case WM_KEYDOWN:

		//右キーが押されたら,ORDERを一つ送る(途中から始める場合)
		if (wParam == VK_RIGHT) {

			movcount++;
			if (movcount > (MOVIENUM - 1)) movcount = 0;
		}

		//ESCキーが押されたら，終了。
		if (wParam == VK_ESCAPE) {

			fclose(fp);
			if (!isDispose) mov.DisposeMovieScreen();//falseなら画面は消えているので省略して良い
			tet.Setparam_i2(-1);
			tet.Setparam_i3(0);
			PostQuitMessage(0);
		}

		//CTRLキーが押されたら，Movie設定＆再生開始
		if (wParam == VK_CONTROL && isPlay==false ) {//再生が止まっている場合のみ発動(誤爆回避)

			if(movcount == MOVIENUM){
				fclose(fp);
				if (!isDispose) mov.DisposeMovieScreen();//falseなら画面は消えているので省略して良い
				tet.Setparam_i2(-1);
				tet.Setparam_i3(0);
				PostQuitMessage(0);
				break;
			}
			//今回の遅延長さは
			DelayLengthSet(movcount);

			mov.SetMovieScreen();

			tet.Setparam_d3(mov.GetCurrentPosition());

			mov.SelectFile(MOVFILEPATH[(MOVIEORDER[movcount])]);

			//アイトラッカの記録開始(終了は動画終了時orESC押下時)
			movname_a = exID + "_" + std::string(timestr) + std::to_string(movcount)+ "_" + std::to_string(delayProgram[1][unit]);
			//この名前で記録開始せよ
			tet.StartListening(movname_a);
			tet.Setparam_i1(0);

			//キー入力の受け渡し先の指定
			//SelectFile，もしくは Startの後ろでないとうまく動かない。
			//ここでは，Global宣言したWindowハンドルを渡しているが，
			//実質WndProcの引数のhwndでも同じもので，どちらを指定しても大丈夫。なはず。
			mov.SetWindowHandle(thewnd);

			//押下時点の時間を記録する
			GetLocalTime(&timenow);
			//char tmpStr[100];
			fprintf(fp, "mov=%S,movcount=%d\n", MOVFILEPATH[MOVIEORDER[movcount]] ,movcount);
			fprintf(fp, "%2d:%02d:%02d.%02d\n",
				timenow.wHour,
				timenow.wMinute,
				timenow.wSecond,
				timenow.wMilliseconds);
			for (int i = 0; i < DELAYCOUNT; i++) { fprintf(fp, "/%f", delayProgram[0][i]);}
			fprintf(fp, "\n/%f\n", delayProgram[1][0]);
			std::string mid = "movie"+ std::to_string(MOVIEORDER[movcount]);
			//std::string mfn = "movie " + mid;
			tet.Setparam_s2(mid);
			tet.Setparam_i2(MOVIEORDER[movcount]);

			//isDisposeのトグル切り替え

			//ここで順番が送られる
			//動画はセット済みなので次に備える
			movcount++;
			//if (movcount > (MOVIENUM - 1)) movcount = 0;
			
			//ここで再生する！
			Sleep(100);
			mov.StartMovie();
			tet.Setparam_s1("mov start");

			isDispose = false;
			isPlay = true;
		}

		if (wParam == VK_SHIFT) {
			//スペースキーが押されたら，一時停止orリスタート
			if (isPlay) {
				if (!isPause) {
					tet.Setparam_s1("KeyPause");
					mov.StopMovie();
					isPause = true;
				}
				else {
					mov.StartMovie();
					isPause = false;
				}
			}
		}

		if (wParam == VK_SPACE) {
			//tet.Setparam_s3("Clicked");
			tet.Setparam_i1(5);
		}

		break;

	case WM_CREATE:
		break;

	case WM_DESTROY:
		mov.DisposeMovieScreen();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
