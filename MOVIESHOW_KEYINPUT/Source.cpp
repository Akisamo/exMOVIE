#include "ms_exp_showmovie.hpp"
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
#define		MOVIECOUNT	2




//画面全体を隠すWindowの作成と，キー入力などによるInteractionの実装
//基本方針としては，Windowsの窓周り関数をそのまま利用
//InterationもOSに依存する
// HINSTANCEやら HWNDやらは，とりあえずおまじないと言うことでw
BSTR			MOVFILEPATH[MOVIECOUNT];
HWND			thewnd;
int				movcount;
SYSTEMTIME		timenow;
FILE			*fp;
errno_t			err;

////インスタンス作成
MSEXP::ShowMov	mov;//例の


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmdline, int cmdshnow){
//int main(void){
	MSG			msg;
	HBRUSH		hbrsh;
	WNDCLASSEX	wcex;
	int			dispx, dispy;

	//プログラム開始時のお約束。音が鳴る。時刻を基に乱数初期化する。
	Beep(577, 100);
	Beep(577, 100);
	srand((unsigned)time(NULL));
	movcount = 0;//何番目の映像か？ってことだよ

	//変数初期化
	MOVFILEPATH[0] = SysAllocString(L"v30.avi");
	MOVFILEPATH[1] = SysAllocString(L"Wildlife.wmv");
	//MOVFILEPATH[3] = SysAllocString(L"");
	//MOVFILEPATH[4] = SysAllocString(L"");
	//MOVFILEPATH[5] = SysAllocString(L"");
	//MOVFILEPATH[6] = SysAllocString(L"");
	//MOVFILEPATH[7] = SysAllocString(L"");
	//MOVFILEPATH[8] = SysAllocString(L"");
	//MOVFILEPATH[9] = SysAllocString(L"");

	//画面サイズの取得　デバッグフラグが有効の場合は，縦横半分で作る
	dispx = GetSystemMetrics(SM_CXSCREEN);
	dispy = GetSystemMetrics(SM_CYSCREEN);
	if (DEBUGMODE){
		dispx = dispx*.75;
		dispy = dispy*.75;
	}

	//親ウィンドウの作成
	//背景塗りようのブラシの作成
	hbrsh = CreateSolidBrush(BGMAIN);

	//タイムスタンプの出力
	err = fopen_s(&fp,"d.txt","w");
	if(err == 0){//開けなかったら
		printf("open\n");
		}
	else{
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
	wcex.hbrBackground = hbrsh;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = classNAME;//窓作成の際に参照されるっぽい。
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	RegisterClassEx(&wcex);

	//窓作成
	thewnd = CreateWindowEx(NULL, classNAME, classNAME, WS_POPUP, 0, 0, dispx, dispy, NULL, NULL, hinst, NULL);
	if (thewnd == NULL){
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
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ShowCursor(true);

	UnregisterClass(classNAME, hinst);
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	//この中身は，OSからのCallによって「その都度」呼び出される。
	//従って，Scopeがこの関数の中だけで終わるものは，その都度初期化される。
	switch (uMsg){

	case WM_PAINT://再描画処理
		break;

	case WM_TIMER:

		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE){
			//ESCキーが押されたら，終了。
			fclose(fp);
			mov.DisposeMovieScreen();
			PostQuitMessage(0);
		}
		if (wParam == VK_CONTROL){
			//CTRLキーが押されたら，Movie設定＆再生開始
			mov.SetMovieScreen();

			mov.SelectFile(MOVFILEPATH[movcount]);
			
			//キー入力の受け渡し先の指定
			//SelectFile，もしくは Startの後ろでないとうまく動かない。
			//ここでは，Global宣言したWindowハンドルを渡しているが，実質WndProcの引数のhwndでも同じもので，どちらを指定しても大丈夫。なはず。
			mov.SetWindowHandle(thewnd);


			movcount++;
			
			if (movcount > (MOVIECOUNT-1)) movcount = 0;
			mov.StartMovie();
					
			//この時点の時間を把握
					GetLocalTime(&timenow);
					//char tmpStr[100];
					fprintf(fp,"%d\n",movcount);
					fprintf(fp,"%2d:%02d:%02d.%02d\n",
						timenow.wHour,
						timenow.wMinute,
						timenow.wSecond,
						timenow.wMilliseconds);
		}
		if (wParam == VK_SPACE){
			//スペースキーが押されたら，一時停止orリスタート
			if (mov.isReadyToPlay()){
				if (mov.isMoviePlaying()){
					mov.StopMovie();
				}
				else{
 					mov.StartMovie();

				}
			}
			else{
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
