#include "ms_exp_showmovie.hpp"
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//�f�o�b�O�t���O ���ꂪON����75���̑傫���ő��쐬
#define		DEBUGMODE	true

//WindowClass���̐ݒ�
#define		classNAME	_T("MYWINDOWDDD")

//�w�i�F�Ƃ��C����Ȃ��̂�Declare
#define		BGMAIN	RGB(127,127,127)

//���搔
#define		MOVIECOUNT	2




//��ʑS�̂��B��Window�̍쐬�ƁC�L�[���͂Ȃǂɂ��Interaction�̎���
//��{���j�Ƃ��ẮCWindows�̑�����֐������̂܂ܗ��p
//Interation��OS�Ɉˑ�����
// HINSTANCE��� HWND���́C�Ƃ肠�������܂��Ȃ��ƌ������Ƃ�w
BSTR			MOVFILEPATH[MOVIECOUNT];
HWND			thewnd;
int				movcount;
SYSTEMTIME		timenow;
FILE			*fp;
errno_t			err;

////�C���X�^���X�쐬
MSEXP::ShowMov	mov;//���


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmdline, int cmdshnow){
//int main(void){
	MSG			msg;
	HBRUSH		hbrsh;
	WNDCLASSEX	wcex;
	int			dispx, dispy;

	//�v���O�����J�n���̂��񑩁B������B��������ɗ�������������B
	Beep(577, 100);
	Beep(577, 100);
	srand((unsigned)time(NULL));
	movcount = 0;//���Ԗڂ̉f�����H���Ă��Ƃ���

	//�ϐ�������
	MOVFILEPATH[0] = SysAllocString(L"v30.avi");
	MOVFILEPATH[1] = SysAllocString(L"Wildlife.wmv");
	//MOVFILEPATH[3] = SysAllocString(L"");
	//MOVFILEPATH[4] = SysAllocString(L"");
	//MOVFILEPATH[5] = SysAllocString(L"");
	//MOVFILEPATH[6] = SysAllocString(L"");
	//MOVFILEPATH[7] = SysAllocString(L"");
	//MOVFILEPATH[8] = SysAllocString(L"");
	//MOVFILEPATH[9] = SysAllocString(L"");

	//��ʃT�C�Y�̎擾�@�f�o�b�O�t���O���L���̏ꍇ�́C�c�������ō��
	dispx = GetSystemMetrics(SM_CXSCREEN);
	dispy = GetSystemMetrics(SM_CYSCREEN);
	if (DEBUGMODE){
		dispx = dispx*.75;
		dispy = dispy*.75;
	}

	//�e�E�B���h�E�̍쐬
	//�w�i�h��悤�̃u���V�̍쐬
	hbrsh = CreateSolidBrush(BGMAIN);

	//�^�C���X�^���v�̏o��
	err = fopen_s(&fp,"d.txt","w");
	if(err == 0){//�J���Ȃ�������
		printf("open\n");
		}
	else{
		printf("error\n");
		return 1;
	}

	//CreateWindowEx���Q�Ƃ��鑋����̏��X�̐ݒ�B�����悭�킩���B
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
	wcex.lpszClassName = classNAME;//���쐬�̍ۂɎQ�Ƃ������ۂ��B
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	RegisterClassEx(&wcex);

	//���쐬
	thewnd = CreateWindowEx(NULL, classNAME, classNAME, WS_POPUP, 0, 0, dispx, dispy, NULL, NULL, hinst, NULL);
	if (thewnd == NULL){
		return 0;
	}

	//���ɑ΂��Ă����܁[�֐��̐ݒ�
	SetTimer(thewnd, 20, 20, NULL);

	//�쐬��������\���i������Ă΂Ȃ��Ă��\������Ă͂���͂��j
	ShowWindow(thewnd, cmdshnow);

	//�}�E�X�J�[�\�������i�������C����̈�͕ʃE�B���h�E�Ȃ̂ł��̐ݒ�͖����ɂȂ�j
	ShowCursor(FALSE);

	//�܂��Ȃ��ȂނȂ�
	UpdateWindow(thewnd);

	// DIRECTSHOW setup



	//�ȉ���While���[�v���@�\���Ă���ԁC�E�B���h�E���\������Ă���B
	// ���̎��Ԃ́COS����̃C�x���g�ɉ����ČĂяo�����  CALLBACK WndProc()������Ă���
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ShowCursor(true);

	UnregisterClass(classNAME, hinst);
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	//���̒��g�́COS�����Call�ɂ���āu���̓s�x�v�Ăяo�����B
	//�]���āCScope�����̊֐��̒������ŏI�����̂́C���̓s�x�����������B
	switch (uMsg){

	case WM_PAINT://�ĕ`�揈��
		break;

	case WM_TIMER:

		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE){
			//ESC�L�[�������ꂽ��C�I���B
			fclose(fp);
			mov.DisposeMovieScreen();
			PostQuitMessage(0);
		}
		if (wParam == VK_CONTROL){
			//CTRL�L�[�������ꂽ��CMovie�ݒ聕�Đ��J�n
			mov.SetMovieScreen();

			mov.SelectFile(MOVFILEPATH[movcount]);
			
			//�L�[���͂̎󂯓n����̎w��
			//SelectFile�C�������� Start�̌��łȂ��Ƃ��܂������Ȃ��B
			//�����ł́CGlobal�錾����Window�n���h����n���Ă��邪�C����WndProc�̈�����hwnd�ł��������̂ŁC�ǂ�����w�肵�Ă����v�B�Ȃ͂��B
			mov.SetWindowHandle(thewnd);


			movcount++;
			
			if (movcount > (MOVIECOUNT-1)) movcount = 0;
			mov.StartMovie();
					
			//���̎��_�̎��Ԃ�c��
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
			//�X�y�[�X�L�[�������ꂽ��C�ꎞ��~or���X�^�[�g
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
