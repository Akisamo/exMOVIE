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
#define		MOVIENUM	2




//��ʑS�̂��B��Window�̍쐬�ƁC�L�[���͂Ȃǂɂ��Interaction�̎���
//��{���j�Ƃ��ẮCWindows�̑�����֐������̂܂ܗ��p
//Interation��OS�Ɉˑ�����
// HINSTANCE��� HWND���́C�Ƃ肠�������܂��Ȃ��ƌ������Ƃ�w
BSTR			MOVFILEPATH[MOVIENUM];
HWND			thewnd;
int				movcount;
SYSTEMTIME		timenow;
FILE			*fp;
errno_t			err;

	//akisamo
    HDC         hdc, hMdc;
    PAINTSTRUCT ps;
    HBITMAP     hbmp;


	//akisamo
	//����Đ��̏��Ԑݒ�
	//FILEPATH�͓��悲�Ƃ̒ʂ��ԍ��AMOVIEORDER�͏��Ԃ������B
	//MOVIEORDER[2]=7�́A2�Ԃ߂�MOVFILEPATH[7]���Đ�����Ƃ�������
	int	MOVIEORDER[MOVIENUM] = {1,0};

////�C���X�^���X�쐬
MSEXP::ShowMov	mov;//��̒ǉ��w�b�_��


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

	movcount = 0;//���Ԗڂ̃Z�N�V�������H

	//akisamo
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

	//akisamo
	//�^�C���X�^���v�̏o��
	char filename[100];
	sprintf_s(filename,"sec-%d_mov%d.txt",movcount,MOVIEORDER[movcount]);

	err = fopen_s(&fp,filename,"w");
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
	wcex.hbrBackground = hbrsh;//�w�i�h���Ă�
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

/*		//akisamo
		//�w�i�ɉ摜��\��������
        hdc= BeginPaint (hwnd, &ps);
		hbmp = (HBITMAP)LoadImage( NULL, "graystart.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
        hMdc= CreateCompatibleDC( hdc );
        SelectObject( hMdc, hbmp );
        
        BitBlt(hdc, 10, 20, 1920, 1080, hMdc, 0, 0, SRCCOPY);
        
        DeleteDC(hMdc);
        DeleteObject( hbmp );
*/

		break;

	case WM_TIMER:

		break;

	case WM_KEYDOWN:


		//�w�i�摜���o�邩�e�X�g
		//���܂������Ȃ��悧�I�I�I
		if (wParam == VK_UP){
        hdc= BeginPaint (hwnd, &ps);
		hbmp = (HBITMAP)LoadImage( NULL, "graystart.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE|LR_CREATEDIBSECTION);
		//�G���[����
        if( hbmp==NULL ){
            //MessageBox(hwnd, "Failed to open file", NAME, MB_OK);
            PostMessage(hwnd, WM_DESTROY, 0, 0);
            EndPaint(hwnd, &ps);
            break;
        hMdc= CreateCompatibleDC( hdc );
        SelectObject( hMdc, hbmp );       
	    BitBlt(hdc, 10, 20, 1920, 1080, hMdc, 0, 0, SRCCOPY);
        DeleteDC(hMdc);
        DeleteObject( hbmp );
		}
		}


		if (wParam == VK_RIGHT){
			//akisamo
			//�E�L�[�������ꂽ��,ORDER�������(�r������n�߂�ꍇ)
			movcount++;
			if (movcount > (MOVIENUM-1)) movcount = 0;
		}

/*		if (wParam == VK_RETURN ){
			//�ЂƂ��悪�I�������炱��������A(�܂��͎������o)
			//�L�^�p��File����āA�X�ɂ����ŊJ���B
			//�ł��邾���Đ�����ŗ]�v�ȕ��ׂ������Ȃ��悤�ɁI
				fclose(fp);
				char filename[100];
				sprintf(filename,"$d.txt",MOVIEORDER[movcount]);
				err = fopen_s(&fp,filename,"w");
				if(err == 0){//�J���Ȃ�������
				printf("open\n");
				}
				else{
				printf("error\n");
				return 1;	
				}

		}
*/

		if (wParam == VK_ESCAPE){
			//ESC�L�[�������ꂽ��C�I���B
			fclose(fp);
			mov.DisposeMovieScreen();
			PostQuitMessage(0);
		}
		if (wParam == VK_CONTROL){
			//CTRL�L�[�������ꂽ��CMovie�ݒ聕�Đ��J�n
			mov.SetMovieScreen();

			mov.SelectFile(MOVFILEPATH[( MOVIEORDER[movcount] )]);
			
			//�L�[���͂̎󂯓n����̎w��
			//SelectFile�C�������� Start�̌��łȂ��Ƃ��܂������Ȃ��B
			//�����ł́CGlobal�錾����Window�n���h����n���Ă��邪�C����WndProc�̈�����hwnd�ł��������̂ŁC�ǂ�����w�肵�Ă����v�B�Ȃ͂��B
			mov.SetWindowHandle(thewnd);

			//�����ŏ��Ԃ�������
			//����̓Z�b�g�ς݂Ȃ̂Ŏ��ɔ�����
			movcount++;
			
			if (movcount > (MOVIENUM-1)) movcount = 0;
			mov.StartMovie();
					
			//�������_�̎��Ԃ��L�^����
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
