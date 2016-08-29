/////////////////////////////////////
//MS_EXP_SHOWMOVIE
//
//			ver 0.6
//				@2016/08/02
//					むつみ
//
/////////////////////////////////////
//Notes::
//  0.6.1 2016/08/08
//		追加：
//			bool isFileSet;
//			isReadyToPlay();
//		変更：
//			SetMovieScreen(),DisposeMovieScreen()をやっぱりpublic関数に

//	0.6: 2016/08/04
//		多少説明を追加
//		廃止：
//			FILE *ff
//			WNDCLASSEX gwc
//			bool isMoviePlaying
//			POINT movTopLeft
//
//			StartMovieFullScreen()
//			GetPosiotnLL()
//			isMovieRunning()
//			MoviePlaying()
//
//		追加：
//			IBasicAudio *pBasicAudio
//			long		audioVolume
//
//			QuitFullScreen()
//			SetFullScreen()
//			GetAudioVolume()
//			SetAudioVolume()
//			MuteAudio();
//			UnMuteAudio();
//			isMoviePlaying()
//
//		変更：
//			SetMovieScreen(),DisposeMovieScreen()をprivate関数に
//	0.5: 2016/08/02
//		Initial release　
/////////////////////////////////////



#ifndef MSEXP_MOV_
#define MSEXP_MOV_

#pragma comment(lib,"strmiids.lib")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <mmsystem.h>
#include <iostream>
#include <dshow.h>
#include <string>
#include <conio.h>


namespace MSEXP{
	class ShowMov{
	private:
		//WindowHandle
		HWND            g_hwnd, tskb, stbtn;
		bool			isTaskBarHidden, isStartButtonHidden, isScreenSet, isFileSet;
		HINSTANCE		hInst;

		//Graph, GraphControl
		IGraphBuilder	*pGraphBuilder;
		IMediaControl	*pMediaControl;
		IMediaPosition	*pMediaPosition;
		IMediaEventEx	*pEvent;
		IVideoWindow	*pVideoWindow;
		IBasicVideo		*pBasicVideo;
		IMediaSeeking	*pMediaSeeking;
		IBasicAudio		*pBasicAudio;
		
		//Movie info, Window Info
		REFTIME			length, mediaLength;
		long			movieHeight, movieWidth, windowHeight, windowWidth, audioVolume;
		int				dx, dy;
		HRESULT			hre;
		OAFilterState	fs;
		GUID			ltimeformat;

	public:
		ShowMov();
		~ShowMov();

		//動画ファイルの指定と初期化
		int		SelectFile(BSTR);

		//pMediaControl->GetState の値を返す。
		//動画が正しく読み込まれたか，停止しているのかなど。あんまり使い物になってない。
		int		GetMovieState(void);

		//タスクバー，および，スタートボタンを隠す
		void	HideTaskBar(void);
		
		//タスクバー，および，スタートボタンを復活させる。
		void	ShowTaskBar(void);

		//再生ができる状態にあるか否かを返す
		bool	isReadyToPlay(void);

		//キー入力等のイベントを， hi で指定されたハンドルに流す
		void	SetWindowHandle(HWND);

		//動画再生
		int		StartMovie(void);

		//動画停止
		int		StopMovie(void);

		//動画が再生中（true）か否（false）を返す
		//判定は，動画の再生時刻と動画の長さの比較に基づく
		bool	isMoviePlaying(void);

		//動画再生時刻（時系列上の位置）を指定する
		void	SetCurrentPosition(REFTIME);

		//現在の動画再生時刻を返す
		REFTIME	GetCurrentPosition(void);

		//動画の長さを返す，
		REFTIME	GetMovieDuration(void);

		//動画の再生レート(速度)を指定する
		//書いては見たものの，ほとんどのフィルタで無効であるらしく，実質機能しない。
		void	SetPlayRate(double);

		//動画の幅を返す（ピクセル）
		long	GetMovieWidth(void);

		//動画の高さを返す（ピクセル）
		long	GetMovieHeight(void);

		//動画を全画面モードに
		void	SetFullScreen(void);

		//動画の全画面モードを終了
		void	QuitFullScreen(void);

		//音声ボリュームを指定
		//値は　（最小）-10,000 ～ 0(最大）で指定。
		void	SetAudioVolume(long);

		//現在の音声ボリュームを返す
		long	GetAudioVolume(void);

		//音声をミュート
		void	MuteAudio(void);

		//音声ミュート解除
		void	UnMuteAudio(void);

		//動画再生ようのあれこれの初期化
		int		SetMovieScreen(void);

		//動画再生ようのあれこれの破棄
		int		DisposeMovieScreen(void);
	};
}


#endif