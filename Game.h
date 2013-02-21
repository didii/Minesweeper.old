/********************************************************************
	created:	2013/02/13
	created:	13:2:2013   9:44
	filename: 	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test\game.h
	file path:	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test
	file base:	game
	file ext:	h
	author:		Dieter Van Broeck
	
	purpose:	Main class which will have control over whether the game is running or
				not. This is the only class called by main.
*********************************************************************/

#include "stdafx.h"

#pragma once
#include "Field.h"

class Game {
	
public:
	enum game_state {UNINITIALIZED, INITIALIZED, SPLASH, MAINMENU, GAME, CLOSING} state;

	// Constructors
	Game();
	~Game();

	// game functions
	bool init();
	int  exec();

	// Game states
	void mainMenu();
	void game();

	void pauze(sf::Event&);
	void rescaleMenu();
	void rescaleGame(Field&);

	///////////////////////////////
	// MEMBER VARIABLES
	// window
	sf::VideoMode VMode;
	sf::RenderWindow win;

	// textures
	sf::Texture std_Tileset;
	float std_size;
	sf::Texture txt_menu;
	sf::Sprite spr_txt_menu;

	// fonts
	sf::Font def_font;

	// field properties
	int height, width, bombs;
	bool debug;
};