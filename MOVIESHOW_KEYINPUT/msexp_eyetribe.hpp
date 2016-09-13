#ifndef MSEXP_EYETRIBE_
#define MSEXP_EYETRIBE_

#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <string>
#include "gazeapi.h"
#include "msexp_misc.hpp"

#define BOOST_ALL_DYN_LINK
#define DATA_HEADDER "JST,pSTR1,pSTR2,pSTR3,pINT1,pINT2,pINT3,pFLO1,pFLO3,pFLO3,COUNT,STATE,FIX,AVG_X,AVG_Y,LEFT_AVG_X,LEFT__AVG_Y,RIGHT_AVG_X,RIGHT_AVG_Y,LEFT_PCENTER_X,LEFT_PCENTER_Y,LEFT_PSIZE,RIGHT_PCENTER_X,RIGHT_PCENTER_Y,RIGHT_PSIZE,LEFT_RAW_X,LEFT_RAW_Y,RIGHT_RAW_X,RIGHT_RAW_Y,RAW_X,RAW_Y,ETELOCALTIME,UTC,\n"
#define EXCUSE_MESSAGE ":::::::::::::::::::::::::::::::::\n:::    EyeTribeSimple         :::\n:::        Written by  ms     :::\n::: ABSOLUTELY NON GUARANTEED :::\n:::::::::::::::::::::::::::::::::\n\n"


namespace MSEXP{
	class EYETRIBE : public gtl::IGazeListener{

	private:
		//���ꂪ���ԁB�p�����Ƃ��Q�Ƃ̂��ƁB
		gtl::GazeApi myapi;

		//�L�^�̉ۂ����߂�t���O
		bool GoOnREC;

		//�f�o�C�X�ڑ����
		bool IsDevOn;

		//���ʋL�^�t�@�C���|�C���^
		FILE *fp;

		//�v���J�E���g
		volatile int framecount;

		//���ʃt�@�C������
		std::string resultfile;

		//�ȉ���param_xx�̓N���X�O����̐��l�󂯓n���̂��߂̕ϐ��B��������Q�b�^�ƃZ�b�^����B
		//����������Cdouble �Ɓ@int����������C�K�v�ɉ����đ��₷�Ȃ�C���Ȃ肷��΂悢�B
		double param_d1, param_d2, param_d3;
		int param_i1, param_i2, param_i3;
		std::string param_s1, param_s2, param_s3;

	public:
		//�R���X�g���N�^ �f�t�H���g
		EYETRIBE();

		//�R���X�g���N�^ ���b�Z�[�W�\���I���\
		EYETRIBE(bool);

		~EYETRIBE(){}
			//�f�R���X�g���N�^�Ȃ̂����E�E�E���ɂ�邱�ƂȂ��̂ł������

	private:
		void on_gaze_data(gtl::GazeData const &);
		// addlistner ���@��������R�[���o�b�N�֐�
		// ���̊֐��́C�ʂ̊֐����Ăяo�����J��Ԃ����Ƃŋ�������B���񖈉�u�V�K�Ɂv�Ăяo�����B
		// �܂�C���̊֐��̒������Ŏw�肳���ϐ��́C���x���x�����������̂ŁC����t���O���ɂ͎g���Ȃ�
		// ���̊֐��̏ꍇ�C�N���XMyGaze�̃����o�ϐ���p���Ă���
	public:
		void StartListening(std::string);
		void EndListening(void);
		void CloseConnection(void);
		void StartCalibration(void);

	public:
		//�ȉ��C���ɂ�����ʂƉ��ʁB���p���l�����邩�ǂ����͂Ƃ������C�p�ӂ����͂��Ă���B 
		int GetFrameCount();
		void SetFrameCount(int);
		bool GetRecFlag(void);
		void SetRecFlag(bool);
		bool GetIsDevOn(void);
		double Getparam_d1(void);
		void Setparam_d1(double);
		double Getparam_d2(void);
		void Setparam_d2(double);
		double Getparam_d3(void);
		void Setparam_d3(double);
		int Getparam_i1(void);
		void Setparam_i1(int);
		int Getparam_i2(void);
		void Setparam_i2(int);
		int Getparam_i3(void);
		void Setparam_i3(int);
		void Setparam_s1(std::string);
		void Setparam_s2(std::string);
		void Setparam_s3(std::string);
		// �ȏ�C���ʂƐ��
	};
}



#endif
