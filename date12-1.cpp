// date12-1.cpp : �������̨Ӧ�ó������ڵ㡣
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

