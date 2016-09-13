//////最終更新　2015/10/12　22:09

#include "msexp_misc.hpp"



void MSEXP::MISC::Waitfor(int msec){
	DWORD STT = GetTickCount();
	while ((GetTickCount() - STT) < msec){
	}
}

void MSEXP::MISC::WaitAnyKey(void){
	while (_kbhit()){
		_getch();
	}
	while (!_kbhit()){
	}
}
 
void MSEXP::MISC::flushKeyPress(void){
	while (_kbhit()){
		_getch();
	}
}

bool MSEXP::MISC::isKeyPressed(void){
	if (_kbhit()){
		return true;
	}
	else{
		return false;
	}
}



std::string MSEXP::MISC::GetTimeString_UCT(void){

	std::string st;
	SYSTEMTIME tm;
	char timestr[13];
	GetSystemTime(&tm);
	sprintf_s(timestr, "%02d:%02d:%02d:%03d",
		tm.wHour,
		tm.wMinute,
		tm.wSecond,
		tm.wMilliseconds
		);
	st = std::string(timestr);
	return(st);
}


std::string MSEXP::MISC::GetTimeString_JST(void){

	std::string st;
	SYSTEMTIME tm;
	char timestr[13];
	GetSystemTime(&tm);
	sprintf_s(timestr, "%02d:%02d:%02d:%03d",
		(tm.wHour+9)%24,
		tm.wMinute,
		tm.wSecond,
		tm.wMilliseconds
		);
	st = std::string(timestr);
	return(st);
}