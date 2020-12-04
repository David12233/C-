// date12-1.cpp : 定义控制台应用程序的入口点。
//



#include "stdafx.h"
#include "game.h"


int main()
{

	initgraph(WIDTH, HEIGHT);

	Game game;
	game.run_main();

	closegraph();

    return 0;
}

