//////最終更新　2015/10/12　22:09


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
		//ちょいとしたギミック集
	public:
		// 何かキーが押されるまで待つ
		static void WaitAnyKey(void);

		//何かキーが押されていたら（呼び出し以前のキャッシュも含む） trueを返す。
		static bool isKeyPressed(void);

		//バッファにたまっているキーイベントをフラッシュ
		static void flushKeyPress(void);

		//協定世界時を返す。UTC。
		static std::string GetTimeString_UCT(void);

		//日本標準時を返す。ただし，日付またぎ近辺では日付が怪しい
		static std::string GetTimeString_JST(void);

		//押されていたキーを返す。ノンブロッキング。
		static char CharOfPressedKey(void);

		//指定したキーが押されるまでブロック
		static void WaitDesignatedKeyPress(char);

		//指定した時間が経過するまでブロック。
		void Waitfor(int);

	};
}


#endif


