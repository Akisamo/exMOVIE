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
		//これが実態。継承もとを参照のこと。
		gtl::GazeApi myapi;

		//記録の可否を決めるフラグ
		bool GoOnREC;

		//デバイス接続状態
		bool IsDevOn;

		//結果記録ファイルポインタ
		FILE *fp;

		//計測カウント
		volatile int framecount;

		//結果ファイル名称
		std::string resultfile;

		//以下のparam_xxはクラス外からの数値受け渡しのための変数。いずれもゲッタとセッタあり。
		//さしあたり，double と　intを作ったが，必要に応じて増やすなり，削るなりすればよい。
		double param_d1, param_d2, param_d3;
		int param_i1, param_i2, param_i3;
		std::string param_s1, param_s2, param_s3;

	public:
		//コンストラクタ デフォルト
		EYETRIBE();

		//コンストラクタ メッセージ表示選択可能
		EYETRIBE(bool);

		~EYETRIBE(){}
			//デコンストラクタなのだが・・・特にやることないのでからっぽ

	private:
		void on_gaze_data(gtl::GazeData const &);
		// addlistner が叩き続けるコールバック関数
		// この関数は，別の関数が呼び出しを繰り返すことで挙動する。毎回毎回「新規に」呼び出される。
		// つまり，この関数の中だけで指定される変数は，毎度毎度初期化されるので，分岐フラグ等には使えない
		// この関数の場合，クラスMyGazeのメンバ変数を用いている
	public:
		void StartListening(std::string);
		void EndListening(void);
		void CloseConnection(void);
		void StartCalibration(void);

	public:
		//以下，俗にいう雪駄と下駄。利用価値があるかどうかはともかく，用意だけはしている。 
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
		// 以上，下駄と雪駄
	};
}



#endif
