//////�ŏI�X�V�@2015/10/12�@22:09


#ifndef MSEXP_MISC_
#define MSEXP_MISC_


#include <conio.h>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

namespace MSEXP{
	class MISC{
		//���傢�Ƃ����M�~�b�N�W
	public:
		// �����L�[���������܂ő҂�
		static void WaitAnyKey(void);

		//�����L�[��������Ă�����i�Ăяo���ȑO�̃L���b�V�����܂ށj true��Ԃ��B
		static bool isKeyPressed(void);

		//�o�b�t�@�ɂ��܂��Ă���L�[�C�x���g���t���b�V��
		static void flushKeyPress(void);

		//���萢�E����Ԃ��BUTC�B
		static std::string GetTimeString_UCT(void);

		//���{�W������Ԃ��B�������C���t�܂����ߕӂł͓��t��������
		static std::string GetTimeString_JST(void);

		//������Ă����L�[��Ԃ��B�m���u���b�L���O�B
		static char CharOfPressedKey(void);

		//�w�肵���L�[���������܂Ńu���b�N
		static void WaitDesignatedKeyPress(char);

		//�w�肵�����Ԃ��o�߂���܂Ńu���b�N�B
		void Waitfor(int);

	};
}


#endif


