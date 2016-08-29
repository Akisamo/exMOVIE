/////////////////////////////////////
//MS_EXP_SHOWMOVIE
//
//			ver 0.6
//				@2016/08/02
//					�ނ�
//
/////////////////////////////////////
//Notes::
//  0.6.1 2016/08/08
//		�ǉ��F
//			bool isFileSet;
//			isReadyToPlay();
//		�ύX�F
//			SetMovieScreen(),DisposeMovieScreen()������ς�public�֐���

//	0.6: 2016/08/04
//		����������ǉ�
//		�p�~�F
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
//		�ǉ��F
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
//		�ύX�F
//			SetMovieScreen(),DisposeMovieScreen()��private�֐���
//	0.5: 2016/08/02
//		Initial release�@
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

		//����t�@�C���̎w��Ə�����
		int		SelectFile(BSTR);

		//pMediaControl->GetState �̒l��Ԃ��B
		//���悪�������ǂݍ��܂ꂽ���C��~���Ă���̂��ȂǁB����܂�g�����ɂȂ��ĂȂ��B
		int		GetMovieState(void);

		//�^�X�N�o�[�C����сC�X�^�[�g�{�^�����B��
		void	HideTaskBar(void);
		
		//�^�X�N�o�[�C����сC�X�^�[�g�{�^���𕜊�������B
		void	ShowTaskBar(void);

		//�Đ����ł����Ԃɂ��邩�ۂ���Ԃ�
		bool	isReadyToPlay(void);

		//�L�[���͓��̃C�x���g���C hi �Ŏw�肳�ꂽ�n���h���ɗ���
		void	SetWindowHandle(HWND);

		//����Đ�
		int		StartMovie(void);

		//�����~
		int		StopMovie(void);

		//���悪�Đ����itrue�j���ہifalse�j��Ԃ�
		//����́C����̍Đ������Ɠ���̒����̔�r�Ɋ�Â�
		bool	isMoviePlaying(void);

		//����Đ������i���n���̈ʒu�j���w�肷��
		void	SetCurrentPosition(REFTIME);

		//���݂̓���Đ�������Ԃ�
		REFTIME	GetCurrentPosition(void);

		//����̒�����Ԃ��C
		REFTIME	GetMovieDuration(void);

		//����̍Đ����[�g(���x)���w�肷��
		//�����Ă͌������̂́C�قƂ�ǂ̃t�B���^�Ŗ����ł���炵���C�����@�\���Ȃ��B
		void	SetPlayRate(double);

		//����̕���Ԃ��i�s�N�Z���j
		long	GetMovieWidth(void);

		//����̍�����Ԃ��i�s�N�Z���j
		long	GetMovieHeight(void);

		//�����S��ʃ��[�h��
		void	SetFullScreen(void);

		//����̑S��ʃ��[�h���I��
		void	QuitFullScreen(void);

		//�����{�����[�����w��
		//�l�́@�i�ŏ��j-10,000 �` 0(�ő�j�Ŏw��B
		void	SetAudioVolume(long);

		//���݂̉����{�����[����Ԃ�
		long	GetAudioVolume(void);

		//�������~���[�g
		void	MuteAudio(void);

		//�����~���[�g����
		void	UnMuteAudio(void);

		//����Đ��悤�̂��ꂱ��̏�����
		int		SetMovieScreen(void);

		//����Đ��悤�̂��ꂱ��̔j��
		int		DisposeMovieScreen(void);
	};
}


#endif