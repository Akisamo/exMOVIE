/////////////////////////////////////
//MS_EXP_SHOWMOVIE
//
//			ver 0.6
//				@2016/08/02
//					�ނ�
//
// 	Release notes on headder file			
/////////////////////////////////////


//hpp�Ő錾�����֐��̒��g���L�q���Ă���B�w�b�_�ƃZ�b�g�ł������
#include "ms_exp_showmovie.hpp"


MSEXP::ShowMov::ShowMov(){
	//�R���X�g���N�^�@�قƂ�ǂ͎��s�̂��߂̉�����

	//�^�X�N�o�[�ƃX�^�[�g�{�^����\��/��\�������邽�߁C�����̃n���h�����擾
	this->isTaskBarHidden = false;
	this->isStartButtonHidden = false;
	this->tskb = FindWindow("Shell_TrayWnd", NULL);
	this->stbtn = FindWindow("Button", NULL);

	//��ʂ̑傫�����擾
	this->dx = GetSystemMetrics(SM_CXSCREEN);
	this->dy = GetSystemMetrics(SM_CYSCREEN);

	//��������C�^�X�N�o�[���B��
	HideTaskBar();


	this->mediaLength = 0;
	this->isScreenSet = false;
	this->isFileSet =false;

};


MSEXP::ShowMov::~ShowMov(){
	//�f�R���X�g���N�^
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
	//�w�肳�ꂽ�t�@�C���̍Đ����ԁi����̒����j���擾
	return this->mediaLength;
};


int MSEXP::ShowMov::SetMovieScreen(void){
	//����̍Đ��̈�₻�̂��߂̃f�o�C�X�C�t�B���^���쐬����B
	
	//Screen�����łɑ��݂���ꍇ�͂����j���B
	if (this->isScreenSet){
		DisposeMovieScreen();
	}

	// OPEN NEW WINDOW
	g_hwnd = CreateWindowEx(NULL, NULL, NULL, WS_POPUP, 0, 0, this->dx, this->dy, NULL, NULL, hInst, NULL);
	ShowWindow(g_hwnd, NULL);
	// Initialize COM component
	CoInitialize(NULL);

	// Graph�𐶐�	
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
	//����̉�����Ԃ�
	return this->movieWidth;
}

long MSEXP::ShowMov::GetMovieHeight(void){
	//����̍�����Ԃ�
	return this->movieHeight;
}


void	MSEXP::ShowMov::MuteAudio(void){
	//Audio���[���ɂ���
	pBasicAudio->get_Volume(&this->audioVolume);
	pBasicAudio->put_Volume(-10000);
}

void MSEXP::ShowMov::SetAudioVolume(long vto){
	pBasicAudio->put_Volume(vto);
}

void MSEXP::ShowMov::UnMuteAudio(void){
	//Audio�������l�C�������̓~���[�g���O�̒l�ɂ���
	pBasicAudio->put_Volume(this->audioVolume);
}


int MSEXP::ShowMov::GetMovieState(void){
	//����̍Đ��X�e�[�^�X��Ԃ��B
	//��肩���Ƃ������C�����������悭�킩���̂Ŏg�p�͐������Ȃ�
	OAFilterState fs;
	HRESULT hr = pMediaControl->GetState(100, &fs);
	return fs;
}


void MSEXP::ShowMov::HideTaskBar(void){
	//�^�X�N�o�[�iWindows��ʉ��j����������
	//�����ł͖߂�Ȃ�
	//�f�o�b�O�̓r����~�Ȃǂ������ꍇ�́C�^�X�N�o�[�������Ȃ���ԂƂȂ�B
	//���̏ꍇ�́C���L��ShowTaskBar�݂̂����s����C���邢�́CPC�ċN���ŁB
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
	//�^�X�N�o�[��\���B�\������Ă����Ԃł����s�\�����C���������N����Ȃ��B
	if (this->isTaskBarHidden){
		ShowWindow(this->tskb, SW_SHOW);
	}
	if (this->isStartButtonHidden){
		ShowWindow(this->stbtn, SW_SHOW);
	}
}

void MSEXP::ShowMov::SetWindowHandle(HWND hi){
	//�L�[���͓��̃C�x���g���C hi �Ŏw�肳�ꂽ�n���h���ɗ���
	pVideoWindow->put_MessageDrain((OAHWND)hi);
}

int MSEXP::ShowMov::SelectFile(BSTR path_movfile){

	// Make Graph:: Initialize Screens and Filters
	SetMovieScreen();

	// �t�@�C���w�聕Render
	pMediaControl->RenderFile(path_movfile);

	//����̍����C���̎擾�iin pixels�j
	this->movieHeight = 0;
	this->movieWidth = 0;
	pBasicVideo->GetVideoSize(&movieWidth, &movieHeight);

	//����̎��ԃt�H�[�}�b�g���擾�B���ۂɂ͗��p���Ă��Ȃ����ǂƂ肠�����c���Ă���B
	pMediaSeeking->GetTimeFormat(&this->ltimeformat);

	//�w�肵������̒������擾
	this->mediaLength = 0;
	this->hre = pMediaPosition->get_Duration(&this->mediaLength);

	//����̉����{�����[�����擾
	this->audioVolume = 0;
	pBasicAudio->get_Volume(&this->audioVolume);

	//�r�f�I��ʂ̕\���ʒu���C��ʂ̒����ɐݒ�
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
	//�Đ��J�n
	pMediaControl->Run();
	return 1;
}


int MSEXP::ShowMov::StopMovie(void){
	//Pause the movie.
	pMediaControl->Pause(); 
	return 1;
}

int MSEXP::ShowMov::DisposeMovieScreen(void){
	//�I�������B

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
	//����̍Đ��ʒu���w�肷��
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