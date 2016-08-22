/////////////////////////////////////
//MS_EXP_SHOWMOVIE
//
//			ver 0.6
//				@2016/08/02
//					むつみ
//
// 	Release notes on headder file			
/////////////////////////////////////


//hppで宣言した関数の中身を記述している。ヘッダとセットであらわれる
#include "ms_exp_showmovie.hpp"


MSEXP::ShowMov::ShowMov(){
	//コンストラクタ　ほとんどは実行のための下準備

	//タスクバーとスタートボタンを表示/非表示させるため，これらのハンドルを取得
	this->isTaskBarHidden = false;
	this->isStartButtonHidden = false;
	this->tskb = FindWindow("Shell_TrayWnd", NULL);
	this->stbtn = FindWindow("Button", NULL);

	//画面の大きさを取得
	this->dx = GetSystemMetrics(SM_CXSCREEN);
	this->dy = GetSystemMetrics(SM_CYSCREEN);

	//いったん，タスクバーを隠す
	HideTaskBar();


	this->mediaLength = 0;
	this->isScreenSet = false;
	this->isFileSet =false;

};


MSEXP::ShowMov::~ShowMov(){
	//デコンストラクタ
	if (isScreenSet){
		DisposeMovieScreen();
	}
	ShowTaskBar();
};


bool MSEXP::ShowMov::isReadyToPlay(void){
	bool RUR=false;
	if (this->isFileSet){
		if (this->isScreenSet){
			RUR = true;
		}
	}
	return RUR;
}


REFTIME MSEXP::ShowMov::GetMovieDuration(void){
	//指定されたファイルの再生時間（動画の長さ）を取得
	return this->mediaLength;
};


int MSEXP::ShowMov::SetMovieScreen(void){
	//動画の再生領域やそのためのデバイス，フィルタを作成する。
	
	//Screenがすでに存在する場合はそれを破棄。
	if (this->isScreenSet){
		DisposeMovieScreen();
	}

	// OPEN NEW WINDOW
	g_hwnd = CreateWindowEx(NULL, NULL, NULL, WS_POPUP, 0, 0, this->dx, this->dy, NULL, NULL, hInst, NULL);
	ShowWindow(g_hwnd, NULL);
	// Initialize COM component
	CoInitialize(NULL);

	// Graphを生成	
	CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID *)&pGraphBuilder);

	//Various interfaces
	pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID *)&pMediaControl);
	pGraphBuilder->QueryInterface(IID_IMediaPosition, (LPVOID *)&pMediaPosition);
	pGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID *)&pVideoWindow);
	pGraphBuilder->QueryInterface(IID_IMediaEventEx, (LPVOID *)&pEvent);
	pGraphBuilder->QueryInterface(IID_IBasicVideo, (LPVOID *)&pBasicVideo);
	pGraphBuilder->QueryInterface(IID_IMediaSeeking, (LPVOID*)&pMediaSeeking);
	pGraphBuilder->QueryInterface(IID_IBasicAudio, (LPVOID *)&pBasicAudio);

	pVideoWindow->put_Owner((OAHWND)g_hwnd);
	this->isScreenSet = true;
	return 0;
}


long MSEXP::ShowMov::GetAudioVolume(void){
	long vol;
	pBasicAudio->get_Volume(&vol);
	return vol;
}

long MSEXP::ShowMov::GetMovieWidth(void){
	//動画の横幅を返す
	return this->movieWidth;
}

long MSEXP::ShowMov::GetMovieHeight(void){
	//動画の高さを返す
	return this->movieHeight;
}


void	MSEXP::ShowMov::MuteAudio(void){
	//Audioをゼロにする
	pBasicAudio->get_Volume(&this->audioVolume);
	pBasicAudio->put_Volume(-10000);
}

void MSEXP::ShowMov::SetAudioVolume(long vto){
	pBasicAudio->put_Volume(vto);
}

void MSEXP::ShowMov::UnMuteAudio(void){
	//Audioを初期値，もしくはミュート直前の値にする
	pBasicAudio->put_Volume(this->audioVolume);
}


int MSEXP::ShowMov::GetMovieState(void){
	//動画の再生ステータスを返す。
	//作りかけというか，何が何だかよくわからんので使用は推奨しない
	OAFilterState fs;
	HRESULT hr = pMediaControl->GetState(100, &fs);
	return fs;
}


void MSEXP::ShowMov::HideTaskBar(void){
	//タスクバー（Windows画面下）を消去する
	//自動では戻らない
	//デバッグの途中停止などをした場合は，タスクバーが見えない状態となる。
	//その場合は，下記のShowTaskBarのみを実行する，あるいは，PC再起動で。
	if (tskb != 0){
		if (stbtn != 0){
			ShowWindow(this->stbtn, SW_HIDE);
			this->isStartButtonHidden = true;
		}
		ShowWindow(this->tskb, SW_HIDE);
		this->isTaskBarHidden = true;
	}
}

void MSEXP::ShowMov::ShowTaskBar(void){
	//タスクバーを表示。表示されている状態でも実行可能だが，実質何も起こらない。
	if (this->isTaskBarHidden){
		ShowWindow(this->tskb, SW_SHOW);
	}
	if (this->isStartButtonHidden){
		ShowWindow(this->stbtn, SW_SHOW);
	}
}

void MSEXP::ShowMov::SetWindowHandle(HWND hi){
	//キー入力等のイベントを， hi で指定されたハンドルに流す
	pVideoWindow->put_MessageDrain((OAHWND)hi);
}

int MSEXP::ShowMov::SelectFile(BSTR path_movfile){

	// Make Graph:: Initialize Screens and Filters
	SetMovieScreen();

	// ファイル指定＆Render
	pMediaControl->RenderFile(path_movfile);

	//動画の高さ，補場の取得（in pixels）
	this->movieHeight = 0;
	this->movieWidth = 0;
	pBasicVideo->GetVideoSize(&movieWidth, &movieHeight);

	//動画の時間フォーマットを取得。実際には利用していないけどとりあえず残してある。
	pMediaSeeking->GetTimeFormat(&this->ltimeformat);

	//指定した動画の長さを取得
	this->mediaLength = 0;
	this->hre = pMediaPosition->get_Duration(&this->mediaLength);

	//動画の音声ボリュームを取得
	this->audioVolume = 0;
	pBasicAudio->get_Volume(&this->audioVolume);

	//ビデオ画面の表示位置を，画面の中央に設定
	pVideoWindow->SetWindowPosition(
		(this->dx - this->movieWidth) / 2,
		(this->dy - this->movieHeight) / 2,
		this->movieWidth,
		this->movieHeight
		);

	// Start playing the movie, but stop :: get ready to be played
	pVideoWindow->put_WindowStyle(WS_CLIPSIBLINGS);
	pVideoWindow->put_WindowStyleEx(WS_EX_TOPMOST);
	pMediaControl->StopWhenReady();
	this->isFileSet = true;

	return 1;
}


void MSEXP::ShowMov::SetFullScreen(void){
	pVideoWindow->put_FullScreenMode(OATRUE);
}

void MSEXP::ShowMov::QuitFullScreen(void){
	pVideoWindow->put_FullScreenMode(OAFALSE);
}

int MSEXP::ShowMov::StartMovie(void){
	//再生開始
	pMediaControl->Run();
	return 1;
}


int MSEXP::ShowMov::StopMovie(void){
	//Pause the movie.
	pMediaControl->Pause(); 
	return 1;
}

int MSEXP::ShowMov::DisposeMovieScreen(void){
	//終了処理。

	//Release interfaces
	pMediaControl->Release();
	pGraphBuilder->Release();
	pMediaSeeking->Release();
	pMediaPosition->Release();
	pEvent->Release();
	pBasicVideo->Release();
	pVideoWindow->Release();
	pBasicAudio->Release();

	//Say goodbye to COM Component
	CoUninitialize();
	this->isScreenSet = false;
	this->isFileSet = false;
	return 0;
}

void MSEXP::ShowMov::SetCurrentPosition(REFTIME jumpto){
	//動画の再生位置を指定する
	pMediaPosition->put_CurrentPosition(jumpto);
}

REFTIME MSEXP::ShowMov::GetCurrentPosition(void){
	//return the current position:: time from the beggening of the movie clip.
	pMediaPosition->get_CurrentPosition(&length);
	return this->length;
}


bool MSEXP::ShowMov::isMoviePlaying(void){
	REFTIME	now;
	bool	isPlaying = true;
	now = GetCurrentPosition();
	if (now >= (this->mediaLength)){
		isPlaying = false;
	}
	else{
		if (GetMovieState() == 1){
			isPlaying = false;
		}
	}
	return isPlaying;
}

void MSEXP::ShowMov::SetPlayRate(double pr){
	pMediaSeeking->SetRate(pr);
}