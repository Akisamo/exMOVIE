#include "msexp_eyetribe.hpp"

void MSEXP::EYETRIBE::on_gaze_data(gtl::GazeData const & gaze_data){
	// addlistner が叩き続けるコールバック関数
	// この関数は，別の関数が呼び出しを繰り返すことで挙動する。毎回毎回「新規に」呼び出される。
	// つまり，この関数の中だけで指定される変数は，毎度毎度初期化されるので，分岐フラグ等には使えない
	// この関数の場合，クラスMyGazeのメンバ変数を用いている
	gtl::GazeData gn;
	if (gaze_data.state){
		//if data pushed from the eyetracker
		gn = gaze_data;
		fprintf_s(this->fp, "%s,%s,%s,%s,%d,%d,%d,%f,%f,%f,%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%s,\n",
			MSEXP::MISC::GetTimeString_JST().c_str(),
			this->param_s1,
			this->param_s2,
			this->param_s3,
			this->param_i1,
			this->param_i2,
			this->param_i3,
			this->param_d1,
			this->param_d2,
			this->param_d3,
			this->framecount,
			gn.state,
			gn.fix,
			gn.avg.x,
			gn.avg.y,
			gn.lefteye.avg.x,
			gn.lefteye.avg.y,
			gn.righteye.avg.x,
			gn.righteye.avg.y,
			gn.lefteye.pcenter.x,
			gn.lefteye.pcenter.y,
			gn.lefteye.psize,
			gn.righteye.pcenter.x,
			gn.righteye.pcenter.y,
			gn.righteye.psize,
			gn.lefteye.raw.x,
			gn.lefteye.raw.y,
			gn.righteye.raw.x,
			gn.righteye.raw.y,
			gn.raw.x,
			gn.raw.y,
			gn.time,
			MSEXP::MISC::GetTimeString_UCT().c_str()
			);
		this->framecount++;
	}
	if (!this->GoOnREC){
		//下の関数を呼び出すことでコールバックが止まる。。。？
		this->EndListening();
	}
}

void MSEXP::EYETRIBE::StartListening(std::string FileName){

	this->GoOnREC = true;//フラグセット

	resultfile = FileName + ".csv";//結果ファイルの名称

	std::cout << "\n....Preparing....\n";
	//std::cout << "==This program will track your eyes until \n Q (case sensitive) has been pressed.\n";

	//open result file;
	if (errno_t filerror = fopen_s(&fp, resultfile.c_str(), "a+")){
		//ファイルオープンエラー：実装したければどぞだ。
		//結果ファイルの上書きを回避するためには，何かしら用意したほうがよい。
	}
	fprintf_s(fp, DATA_HEADDER);//ファイルの頭にラベル

	SetFrameCount(0);//カウンタ初期化

	//この下で実質の計測開始
	//add_listner()内部でスレッドたててコールバック関数ぶん回す
	//処理はスタックしない（戻ってくる）
	myapi.add_listener(*this);
	std::cout << "\n::::::Recording Started.\n";
}

void MSEXP::EYETRIBE::EndListening(void){
	myapi.remove_listener(*this);
	fclose(fp);
	//std::cout << "\n::::::Recording Terminated. Result file closed.\n";
}

void MSEXP::EYETRIBE::CloseConnection(void){
	myapi.disconnect();
	//std::cout << "\nConnection to the EyeTribe Server closed.\n\n";
}


void MSEXP::EYETRIBE::StartCalibration(void){
	myapi.calibration_start(9);

}


int MSEXP::EYETRIBE::GetFrameCount(void){
	return(this->framecount);
}

void MSEXP::EYETRIBE::SetFrameCount(int newFrameCount){
	this->framecount = newFrameCount;
}

bool MSEXP::EYETRIBE::GetRecFlag(void){
	//Getter for GoOnREC
	return(this->GoOnREC);
}

void MSEXP::EYETRIBE::SetRecFlag(bool setflag){
	//Setter for GoOnREC
	this->GoOnREC = setflag;
}

bool MSEXP::EYETRIBE::GetIsDevOn(void){
	//IsDevIOnだけは外部からセットされると困るので下駄のみ。
	return(this->IsDevOn);
}

double MSEXP::EYETRIBE::Getparam_d1(void){
	return(this->param_d1);
}

void MSEXP::EYETRIBE::Setparam_d1(double setto){
	this->param_d1 = setto;
}

double MSEXP::EYETRIBE::Getparam_d2(void){
	return(this->param_d2);
}

void MSEXP::EYETRIBE::Setparam_d2(double setto){
	this->param_d2 = setto;
}

double MSEXP::EYETRIBE::Getparam_d3(void){
	return(this->param_d3);
}

void MSEXP::EYETRIBE::Setparam_d3(double setto){
	this->param_d3 = setto;
}

int MSEXP::EYETRIBE::Getparam_i1(void){
	return(this->param_i1);
}

void MSEXP::EYETRIBE::Setparam_i1(int setto){
	this->param_i1 = setto;
}

int MSEXP::EYETRIBE::Getparam_i2(void){
	return this->param_i2;
}

void MSEXP::EYETRIBE::Setparam_i2(int setto){
	this->param_i2 = setto;
}

int MSEXP::EYETRIBE::Getparam_i3(void){
	return this->param_i3;
}

void MSEXP::EYETRIBE::Setparam_i3(int setto){
	this->param_i3 = setto;
}

void MSEXP::EYETRIBE::Setparam_s1(std::string ns){
	this->param_s1 = ns;
}

void MSEXP::EYETRIBE::Setparam_s2(std::string ns){
	this->param_s2 = ns;
}
void MSEXP::EYETRIBE::Setparam_s3(std::string ns){
	this->param_s3 = ns;
}

MSEXP::EYETRIBE::EYETRIBE(){
	if (myapi.connect()){
		this->IsDevOn = true;
	}
	else{
		//接続NG
		std::cout << "<-Could not connect to the EyeTribe server.\n";
		this->IsDevOn = false;
	}
	//初期化
	param_d1 = 0.0;
	param_d2 = 0.0;
	param_d3 = 0.0;
	param_i1 = 0;
	param_i2 = 0;
	param_i3 = 0;
	this->param_s1 = "";
	this->param_s2 = "";
	this->param_s3 = "";

}

//コンストラクタ メッセージ表示選択可能
MSEXP::EYETRIBE::EYETRIBE(bool showmessage = true){
	if (showmessage){
		std::cout << EXCUSE_MESSAGE;
		std::cout << "Tryng to connect the EyeTribe Server.\n";
	}
	if (myapi.connect()){
		//接続オッケー
		if (showmessage){
			std::cout << "->Connected to the EyeTribe server. ->->Let's GO.\n";
		}
		this->IsDevOn = true;
	}
	else{
		//接続NG
		std::cout << "<-Could not connect to the EyeTribe server.\n";
		this->IsDevOn = false;
	}

	//初期化
	param_d1 = 0.0;
	param_d2 = 0.0;
	param_d3 = 0.0;
	param_i1 = 0;
	param_i2 = 0;
	param_i3 = 0;
}