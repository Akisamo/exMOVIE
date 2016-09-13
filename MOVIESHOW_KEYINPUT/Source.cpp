#include "ms_exp_showmovie.hpp"
#include "msexp_misc.hpp"
#include "msexp_eyetribe.hpp"
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//デバッグフラグ これがONだと75％の大きさで窓作成
#define		DEBUGMODE	true

//WindowClass名の設定
#define		classNAME	_T("MYWINDOWDDD")

//背景色とか，そんなもののDeclare
#define		BGMAIN	RGB(127,127,127)

//動画数
#define		MOVIENUM	3




//画面全体を隠すWindowの作成と，キー入力などによるInteractionの実装
//基本方針としては，Windowsの窓周り関数をそのまま利用
//InterationもOSに依存する
// HINSTANCEやら HWNDやらは，とりあえずおまじないと言うことでw
BSTR			MOVFILEPATH[MOVIENUM];
HWND			thewnd;
int				movcount;
SYSTEMTIME		timenow;
FILE			*fp;
errno_t			err;
bool			flgStop;
bool			st;



//akisamo
//動画再生の順番設定
//FILEPATHは動画ごとの通し番号、MOVIEORDERは順番を示す。
//MOVIEORDER[2]=7は、2番めにMOVFILEPATH[7]を再生せよということ
int	MOVIEORDER[MOVIENUM] = { 2,1,0 };

////インスタンス作成
MSEXP::ShowMov	mov;//例の追加ヘッダの
MSEXP::EYETRIBE tet;


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmdline, int cmdshnow) {
	//int main(void){
	MSG			msg;
	HBRUSH		hbrsh;
	WNDCLASSEX	wcex;
	int			dispx, dispy;

	//プログラム開始時のお約束。音が鳴る。時刻を基に乱数初期化する。
	Beep(577, 100);
	Beep(577, 100);
	srand((unsigned)time(NULL));

	movcount = 0;//何番目のセクションか？
	st = false;//動画終了初回のみ通る処理用。再生時はTrueに戻さなければいけない

	//akisamo
	//変数初期化
	MOVFILEPATH[0] = SysAllocString(L"../mov/v30.avi");
	MOVFILEPATH[1] = SysAllocString(L"../mov/Wildlife.wmv");
	MOVFILEPATH[2] = SysAllocString(L"../mov/cookie.avi");
		//MOVFILEPATH[4] = SysAllocString(L"");
		//MOVFILEPATH[5] = SysAllocString(L"");
		//MOVFILEPATH[6] = SysAllocString(L"");
		//MOVFILEPATH[7] = SysAllocString(L"");
		//MOVFILEPATH[8] = SysAllocString(L"");
		//MOVFILEPATH[9] = SysAllocString(L"");


	//EYETRIBE setup
	//tet.StartListening("C:\\Users\\mutsumi\\Desktop\\TESTDAT");
	tet.StartListening("TESTDAT");
	Sleep(2000);
	tet.Setparam_i1(0);


	//画面サイズの取得　デバッグフラグが有効の場合は，縦横半分で作る
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
	//タイムスタンプの出力
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
	HDC         hdc, hMdc;
	PAINTSTRUCT ps;
	HBITMAP     hbmp;
	LPTSTR		stms = TEXT("うりゃ");
	HBRUSH		br;

	switch (uMsg) {



	case WM_PAINT://再描画処理
				  if (st) {
				  Beep(750, 100);
				  hdc = BeginPaint(hwnd, &ps);
				  SetBkColor(hdc, RGB(128, 128, 128));
				  SetTextColor(hdc, RGB(0, 0, 0));
				  int lineH = 40;
				  int Fline = 20;
				  TextOut(hdc, 100, Fline + lineH, stms, strlen(stms));
				  EndPaint(hwnd, &ps);
				  }
				  //akisamo
				  //背景に画像を表示したい
				  
				  hdc= BeginPaint (hwnd, &ps);
				  hbmp = (HBITMAP)LoadImage( NULL, "graystart.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
				  hMdc= CreateCompatibleDC( hdc );
				  SelectObject( hMdc, hbmp );

				  BitBlt(hdc, 10, 20, 1920, 1080, hMdc, 0, 0, SRCCOPY);

				  DeleteDC(hMdc);
				  DeleteObject( hbmp );
				  EndPaint(hwnd, &ps);
				  

		break;

	case WM_TIMER:

		tet.Setparam_d1(mov.GetCurrentPosition());

		if (!mov.isMoviePlaying()) {
			if (!st) {
				mov.DisposeMovieScreen();
				InvalidateRect(hwnd, NULL, TRUE);
				st = true;
			}
		}
		break;

	case WM_KEYDOWN:


		//背景画像が出るかテスト
		//うまくいかないよぉ！！！
		if (wParam == VK_UP) {
			hdc = BeginPaint(hwnd, &ps);
			hbmp = (HBITMAP)LoadImage(NULL, "graystart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			//エラー処理
			if (hbmp == NULL) {
				//MessageBox(hwnd, "Failed to open file", NAME, MB_OK);
				PostMessage(hwnd, WM_DESTROY, 0, 0);
				EndPaint(hwnd, &ps);
				break;
				hMdc = CreateCompatibleDC(hdc);
				SelectObject(hMdc, hbmp);
				BitBlt(hdc, 10, 20, 1920, 1080, hMdc, 0, 0, SRCCOPY);
				DeleteDC(hMdc);
				DeleteObject(hbmp);
			}
		}


		if (wParam == VK_RIGHT) {
			//akisamo
			//右キーが押されたら,ORDERを一つ送る(途中から始める場合)
			movcount++;
			if (movcount > (MOVIENUM - 1)) movcount = 0;
		}

		/*		if (wParam == VK_RETURN ){
		//ひとつ動画が終了したらこれを押し、(または自動検出)
		//記録用のFileを閉じて、更にここで開く。
		//できるだけ再生周りで余計な負荷をかけないように！
		fclose(fp);
		char filename[100];
		sprintf(filename,"$d.txt",MOVIEORDER[movcount]);
		err = fopen_s(&fp,filename,"w");
		if(err == 0){//開けなかったら
		printf("open\n");
		}
		else{
		printf("error\n");
		return 1;
		}

		}
		*/

		if (wParam == VK_ESCAPE) {
			//ESCキーが押されたら，終了。
			fclose(fp);
			mov.DisposeMovieScreen();
			tet.Setparam_i2(-1);
			tet.Setparam_i3(0);
			PostQuitMessage(0);
		}
		if (wParam == VK_CONTROL) {
			//CTRLキーが押されたら，Movie設定＆再生開始
			mov.SetMovieScreen();

			tet.Setparam_d3(mov.GetCurrentPosition());

			mov.SelectFile(MOVFILEPATH[(MOVIEORDER[movcount])]);

			//キー入力の受け渡し先の指定
			//SelectFile，もしくは Startの後ろでないとうまく動かない。
			//ここでは，Global宣言したWindowハンドルを渡しているが，実質WndProcの引数のhwndでも同じもので，どちらを指定しても大丈夫。なはず。
			mov.SetWindowHandle(thewnd);

			//ここで順番が送られる
			//動画はセット済みなので次に備える
			movcount++;
			if (movcount > (MOVIENUM - 1)) movcount = 0;

			Sleep(2000);
			mov.StartMovie();
			tet.Setparam_s1("mov start");

			//押下時点の時間を記録する
			GetLocalTime(&timenow);
			//char tmpStr[100];
			fprintf(fp, "%d\n", movcount);
			fprintf(fp, "%2d:%02d:%02d.%02d\n",
				timenow.wHour,
				timenow.wMinute,
				timenow.wSecond,
				timenow.wMilliseconds);

			std::string mid = std::to_string(movcount);
			std::string mfn = "movie " + mid;
			tet.Setparam_s2(mfn);
			tet.Setparam_i2(movcount);

			//stのトグル切り替え
			st = false;
		}
		if (wParam == VK_SPACE) {
			//スペースキーが押されたら，一時停止orリスタート
			if (mov.isReadyToPlay()) {
				if (mov.isMoviePlaying()) {
					tet.Setparam_s1("KEY PRESSED");
					mov.StopMovie();
				}
				else {
					mov.StartMovie();

				}
			}
			else {
			}
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
