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
using namespace std;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//�f�o�b�O�t���O ���ꂪON����75���̑傫���ő��쐬
#define		DEBUGMODE	true

//WindowClass���̐ݒ�
#define		classNAME	_T("MYWINDOWDDD")

//�w�i�F�Ƃ��C����Ȃ��̂�Declare
#define		BGMAIN	RGB(127,127,127)

//���搔
#define		MOVIENUM	3




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
bool			flgStop;
bool			st;

std::string movname_a;

//����Đ��̏��Ԑݒ�
//FILEPATH�͓��悲�Ƃ̒ʂ��ԍ��AMOVIEORDER�͏��Ԃ������B
//MOVIEORDER[2]=7�́A2�Ԃ߂�MOVFILEPATH[7]���Đ�����Ƃ�������
int	MOVIEORDER[MOVIENUM] = { 2,1,0 };

////�C���X�^���X�쐬
MSEXP::ShowMov	mov;//��̒ǉ��w�b�_
MSEXP::EYETRIBE tet;//�A�C�g���C�u


int WINAPI WinMain(HINSTANCE hinst, HINSTANCE pinst, LPSTR cmdline, int cmdshnow) {
	//int main(void){
	MSG			msg;
	HBRUSH		hbrsh;
	WNDCLASSEX	wcex;
	int			dispx, dispy;

	//�v���O�����J�n���̂��񑩁B������B��������ɗ�������������B
	Beep(577, 100);
	Beep(577, 100);
	srand((unsigned)time(NULL));
	
		//���Ԗڂ̃Z�N�V�������H
		movcount = 0;
		//����I������̂ݒʂ鏈���p�t���O�B
		//�Đ�����True�ɖ߂��B�Đ��I���̏������I�������False�ɐ؂�ւ���
		st = false;

		//akisamo
		//����\�[�X�Ǘ��B��{�I�ɂ͓���ǉ����ȊO�G��Ȃ�
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
	tet.StartListening("TEST1");
	Sleep(2000);
	tet.Setparam_i1(0);


	//��ʃT�C�Y�̎擾�@�f�o�b�O�t���O���L���̏ꍇ�́C�c�������ō��
	dispx = GetSystemMetrics(SM_CXSCREEN);
	dispy = GetSystemMetrics(SM_CYSCREEN);
	if (DEBUGMODE) {
		dispx = dispx*.75;
		dispy = dispy*.75;
	}

	//�e�E�B���h�E�̍쐬
	//�w�i�h��悤�̃u���V�̍쐬
	hbrsh = CreateSolidBrush(BGMAIN);

	//akisamo
	//�^�C���X�^���vfile�̏o��
	char filename[100];
	sprintf_s(filename, "sec-%d_mov%d.txt", movcount, MOVIEORDER[movcount]);

	err = fopen_s(&fp, filename, "w");
	if (err == 0) {//�J���Ȃ�������
		printf("open\n");
	}
	else {
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
	if (thewnd == NULL) {
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
	//���̒��g�́COS�����Call�ɂ���āu���̓s�x�v�Ăяo�����B
	//�]���āCScope�����̊֐��̒������ŏI�����̂́C���̓s�x�����������B

	//akisamo
	HDC         hdc, hMdc, hdcBmp;
	PAINTSTRUCT ps;
	HBITMAP     hbmp;
	LPTSTR		stms = TEXT("�����");
	HBRUSH		br;

	switch (uMsg) {

	case WM_PAINT://�ĕ`�揈��


		if (st) {

			//�v���̏I���Afile���J�������Čv���X�^�[�g
			tet.EndListening();
			Sleep(1000);
			//movname_a = std::to_string(movcount);
			//tet.StartListening(movname_a);
			//Sleep(2000);
			//tet.Setparam_i1(0);

			//��ʏ����Ɖ�������
			Beep(750, 100);
			hdc = BeginPaint(hwnd, &ps);
			SetBkColor(hdc, RGB(128, 128, 128));
			SetTextColor(hdc, RGB(0, 0, 0));
			int lineH = 40;
			int Fline = 20;
			TextOut(hdc, 100, Fline + lineH, stms, strlen(stms));
			EndPaint(hwnd, &ps);

			//akisamo
			//�w�i�ɉ摜��\��������
			hdcBmp = BeginPaint(hwnd, &ps);
			hbmp = (HBITMAP)LoadImage(NULL, "graystart.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			hMdc = CreateCompatibleDC(hdcBmp);
			SelectObject(hMdc, hbmp);

			BitBlt(hdcBmp, 0, 0, 1920, 1080, hMdc, 0, 0, SRCCOPY);

			DeleteDC(hMdc);
			DeleteObject(hbmp);
			EndPaint(hwnd, &ps);
		}

		break;

	case WM_TIMER:

		tet.Setparam_d1(mov.GetCurrentPosition());
		//�����Đ����Ă��Ȃ�������(�Đ��I�����Ă�����)
		//�����ʂ���鏈�����s��
		if (!mov.isMoviePlaying()) {
			if (!st) {//��񂾂��s������ȍ~��Dispose���Ȃ�
				mov.DisposeMovieScreen();
				InvalidateRect(hwnd, NULL, TRUE);
				st = true;
			}
		}
		break;

	case WM_KEYDOWN:



		if (wParam == VK_RIGHT) {
			//akisamo
			//�E�L�[�������ꂽ��,ORDER�������(�r������n�߂�ꍇ)
			movcount++;
			if (movcount > (MOVIENUM - 1)) movcount = 0;
		}



		if (wParam == VK_ESCAPE) {
			//ESC�L�[�������ꂽ��C�I���B
			fclose(fp);
			if (!st) mov.DisposeMovieScreen();//false�Ȃ��ʂ͏����Ă���̂ŏȗ����ėǂ�
			tet.Setparam_i2(-1);
			tet.Setparam_i3(0);
			PostQuitMessage(0);
		}
		if (wParam == VK_CONTROL) {
			//CTRL�L�[�������ꂽ��CMovie�ݒ聕�Đ��J�n
			mov.SetMovieScreen();

			tet.Setparam_d3(mov.GetCurrentPosition());

			mov.SelectFile(MOVFILEPATH[(MOVIEORDER[movcount])]);

			//�A�C�g���b�J�̋L�^�J�n(�I���͓���I����orESC������)
			movname_a = std::to_string(movcount);
			tet.StartListening(movname_a);
			Sleep(2000);
			tet.Setparam_i1(0);

			//�L�[���͂̎󂯓n����̎w��
			//SelectFile�C�������� Start�̌��łȂ��Ƃ��܂������Ȃ��B
			//�����ł́CGlobal�錾����Window�n���h����n���Ă��邪�C
			//����WndProc�̈�����hwnd�ł��������̂ŁC�ǂ�����w�肵�Ă����v�B�Ȃ͂��B
			mov.SetWindowHandle(thewnd);

			//�����ŏ��Ԃ�������
			//����̓Z�b�g�ς݂Ȃ̂Ŏ��ɔ�����
			movcount++;
			if (movcount > (MOVIENUM - 1)) movcount = 0;

			Sleep(2000);
			mov.StartMovie();
			tet.Setparam_s1("mov start");

			//�������_�̎��Ԃ��L�^����
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

			//st�̃g�O���؂�ւ�
			st = false;
		}


		if (wParam == VK_SPACE) {
			//�X�y�[�X�L�[�������ꂽ��C�ꎞ��~or���X�^�[�g
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
