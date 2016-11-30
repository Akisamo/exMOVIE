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
#define		DEBUGMODE	true

//WindowClass名の設定
#define		classNAME	_T("MYWINDOWDDD")

//背景色とか，そんなもののDeclare
#define		BGMAIN	RGB(127,127,127)

//動画数
#define		MOVIENUM	7

//遅延回数
#define     DELAYCOUNT     5

#define		DELAYTIME   5000.0





//画面全体を隠すWindowの作成と，キー入力などによるInteractionの実装
//基本方針としては，Windowsの窓周り関数をそのまま利用
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
BSTR			MOVFILEPATH[MOVIENUM];//動画の場所

int unit, cnt, cnt2;
DOUBLE tempN;
bool flugC;




//動画ごとET記録ファイル命名規則
std::string exID = "test2";

//ある動画中でどのタイミングでどういう長さの遅延が起こるかを定義したもの
//今入っているのは初期化
//動画が切り替わるたびに、1行目をRondomdelay()で、2行目をDelaytimeset()で切り替えていく
DOUBLE		delayProgram[2][DELAYCOUNT+1] = {	{0.5 ,1.0 ,1.5 ,2.0	,2.5 ,100.0 },
										{DELAYTIME,DELAYTIME,DELAYTIME,DELAYTIME,DELAYTIME,999.9 }};//遅延のスクリプト

DOUBLE		delayTiming[DELAYCOUNT] = { 0,0,0,0,0 };
DOUBLE		delayLength[7] = { 0,0,200,500,1000,2000,5000, };

std::string movname_a;

//動画再生の順番設定
//FILEPATHは動画ごとの通し番号、MOVIEORDERは順番を示す。
//MOVIEORDER[2]=7は、2番めにMOVFILEPATH[7]を再生せよということ
int	MOVIEORDER[MOVIENUM] = { 0,0,0,0,0,0,0,};

////インスタンス作成
MSEXP::ShowMov	mov;//例の追加ヘッダ
MSEXP::EYETRIBE tet;//アイトライブ

//乱数
std::random_device rnd;
std::mt19937 mt(rnd());
std::uniform_int_distribution<> rand29(1, 59);//乱数の範囲ここで決まってます

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

	//遅延帳並び替え
	ShuffleDelayLength(delayLength, 7);
	//ランダム
	//Delayタイミングの作成
	RandomDelay();

	//ファイル名
	struct tm now;
	time_t longtime;
	char timestr[32];

	longtime = time(NULL);
	localtime_s(&now, &longtime);
	asctime_s(timestr, sizeof(timestr), &now);
	


	//動画ソース管理。基本的には動画追加時以外触らないで、再生順はMOVIEORDERで入れ替える
	MOVFILEPATH[0] = SysAllocString(L"../mov/Wildlife.wmv");
	MOVFILEPATH[1] = SysAllocString(L"../mov/cookie.avi");
	//MOVFILEPATH[2] = SysAllocString(L"../mov/cookie.avi");
	//MOVFILEPATH[3] = SysAllocString(L"../mov/cookie.avi");
	//MOVFILEPATH[4] = SysAllocString(L"../mov/Wildlife.wmv");
	//MOVFILEPATH[5] = SysAllocString(L"../mov/Wildlife.wmv");
	//MOVFILEPATH[6] = SysAllocString(L"../mov/Wildlife.wmv");
	//MOVFILEPATH[7] = SysAllocString(L"");
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
	sprintf_s(filename, "sec-%d_mov%d.txt", movcount, MOVIEORDER[movcount]);

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


		if (isDispose) {
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

		tet.Setparam_s1("playing");

		tet.Setparam_d1(mov.GetCurrentPosition());
		//再生終了した初回のみ
		//動画画面を閉じる処理を行う
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
				tet.Setparam_s1("Delay_"+ std::to_string(delayProgram[0][unit]));
				tet.Setparam_d2(delayProgram[1][unit]);
				mov.StopMovie();
				Sleep(delayProgram[1][unit]);
				mov.StartMovie();
				tet.Setparam_s1("ReStart");
				//tet.Setparam_i3(unit);
				unit = unit+1;
					}
		break;

	case WM_KEYUP:
		if(wParam == VK_SPACE){	tet.Setparam_s3(""); }
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
		if (wParam == VK_CONTROL) {

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
			movname_a = exID + "_" + std::to_string(movcount);
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
			fprintf(fp, "\n\nmov=%S,movcount=%d\n", MOVFILEPATH[MOVIEORDER[movcount]] ,movcount);
			fprintf(fp, "%2d:%02d:%02d.%02d\n",
				timenow.wHour,
				timenow.wMinute,
				timenow.wSecond,
				timenow.wMilliseconds);
			for (int i = 0; i < DELAYCOUNT; i++) { fprintf(fp, "/%f", delayProgram[0][i]);}
			fprintf(fp, "\n/%f", delayProgram[1][0]);
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
			tet.Setparam_s3("Clicked");
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
