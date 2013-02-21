// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma once
#include "Game.h"


int _tmain(int argc, _TCHAR* argv[]) {

	srand(time(NULL));


	Game game;
	if (!game.init())
		return -1;

	return game.exec();
}

