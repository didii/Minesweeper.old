/********************************************************************
	created:	13/02/2013   22:23
	filename: 	C:\Users\Dieter\Documents\Visual Studio 2010\Projects\Test\Test\Game.cpp
	file path:	C:\Users\Dieter\Documents\Visual Studio 2010\Projects\Test\Test
	file base:	Game
	file ext:	cpp
	author:		Dieter Van Broeck
	
	purpose:	Definitions of file:game.h
*********************************************************************/
#include "stdafx.h"

#include "Game.h"
#include "Field.h"


Game::Game() {
	state = UNINITIALIZED;
}
Game::~Game() {}

bool Game::init() {
	/////////////////////////////////////////////////////
	// Load up some files
	/////////////////////////////////////////////////////
	if (!std_Tileset.loadFromFile("Tileset.png")) {
		cout << "Failed to load Tilesset.png\n";
		return false;
	}
	std_size = 64;
	if (!txt_menu.loadFromFile("MenuText.png")) {
		cout << "Failed to load MenuText.png";
		return false;
	}
	spr_txt_menu.setTexture(txt_menu);
	spr_txt_menu.setPosition(100,100);
	if (!def_font.loadFromFile("Font.ttf")) {
		cout << "Failed to load Font.ttf\n";
		return false;
	}
	/////////////////////////////////////////////////////



	/////////////////////////////////////////////////////
	// Setup some variables
	/////////////////////////////////////////////////////
	height = 16;
	width = 16;
	bombs = 40;
	/////////////////////////////////////////////////////

	state = INITIALIZED;

	return true;
}
int Game::exec() {
	if (state == UNINITIALIZED) {
		cout << "Error: Game state still UNINITIALIZED";
		return -1;
	}	
	if (state == INITIALIZED)
		state = MAINMENU;


	// enter main loop
	while (state != CLOSING) {
		switch (state) {
			case MAINMENU:
				mainMenu();
				break;
			case GAME:
				game();
				break;
			default:
				cout << "Error: wrong state of game";
				return -1;
				break;
		}
	}

	return 0;
}

void Game::mainMenu() {
	rescaleMenu();

	debug = false;
	sf::Text txt_field;
	txt_field.setFont(def_font);
	txt_field.setColor(sf::Color::White);
	txt_field.setPosition(100,400);
	txt_field.setCharacterSize(24);


	win.setFramerateLimit(60);

	cout << "Read the instructions and start playing some minesweeper!\n\n";

	sf::Event ev;
	while (state == MAINMENU) {
		while (win.pollEvent(ev)) {
			if (ev.type == sf::Event::KeyPressed) {
				switch (ev.key.code) {
					case sf::Keyboard::Escape:
						state = CLOSING;
						break;
					case sf::Keyboard::Up:
						height++;
						break;
					case sf::Keyboard::Down:
						height--;
						break;
					case sf::Keyboard::Right:
						width++;
						break;
					case sf::Keyboard::Left:
						width--;
						break;
					case sf::Keyboard::PageUp:
						bombs++;
						break;
					case sf::Keyboard::PageDown:
						bombs--;
						break;
					case sf::Keyboard::Num1:
						width = 9;
						height = 9;
						bombs = 10;
						break;
					case sf::Keyboard::Num2:
						width = 16;
						height = 16;
						bombs = 40;
						break;
					case sf::Keyboard::Num3:
						width = 30;
						height = 16;
						bombs = 99;
						break;
					case sf::Keyboard::Return:
						state = GAME;
						break;
					case sf::Keyboard::Space:
						state = GAME;
						break;
					case sf::Keyboard::F8:
						debug = true;
						state = GAME;
					default:
						break;
				}
			}
			if (height <= 0) {
				height = 1;
			}
			if (width <= 0) {
				width = 1;
			}
			if (bombs <= 0) {
				bombs = 1;
			}
			if (bombs > height*width) {
				bombs = height*width;
			}
			ostringstream temp;
			temp << "Height: " << height << "\n\nWidth: " << width << "\n\nBombs: " << bombs;
			txt_field.setString(temp.str());

			win.clear();
			win.draw(spr_txt_menu);
			win.draw(txt_field);
			win.display();
		}
	}
	return;
}
void Game::game() {
	Field field(width, height, bombs, debug);
	field.setTexture("Tileset.png");
	field.setTextureTileSize(64);
	field.setDrawTileSize(64);
	field.setWindow(&win);
	rescaleGame(field);
	cout << "\n\n\nGame started, good luck!\n\n";

	field.create();

	// check if field takes up more space than your current resolution

	while (state == GAME) {
		sf::Event ev;
		while (win.pollEvent(ev)) {
			if (ev.type == sf::Event::KeyPressed) {
				switch (ev.key.code) {
					case sf::Keyboard::Escape:
						state = CLOSING;
						break;
					case sf::Keyboard::H:
						field.giveHint();
						break;
					default:
						break;
				}
			}
			field.update(ev);
		}
		if (field.isLost()) {
			cout << "You lost the game!\nPress any key to return to main menu.\n\n\n";
			state = MAINMENU;
			field.setAllRevealState(true);
			field.updateAllVertex(true);
		}
		if (field.isWon()) {
			cout << "Congratulations, You won!\nPress any key to return to main menu.\n\n\n";
			state = MAINMENU;
			field.setAllRevealState(true);
			field.updateAllVertex(true);
		}
		
		win.clear(sf::Color::White);
		field.draw();
		win.display();

		if (field.isLost() || field.isWon()) {
			pauze(ev);
		}
	}



	state = MAINMENU;
	return;
}

void Game::pauze(sf::Event &ev) {
	win.waitEvent(ev);
	if (ev.type == sf::Event::KeyReleased)
		return;
	else
		pauze(ev);
}

void Game::rescaleMenu() {
	VMode = sf::VideoMode(800,600);
	if (win.isOpen())
		win.close();
	win.create(VMode, "Minesweeper - SFML");
	win.setFramerateLimit(10);
}
void Game::rescaleGame(Field &field){
	sf::Vector2u win_size(width*field.getDrawTileSize(), height*field.getDrawTileSize());
	sf::VideoMode desk_VM(sf::VideoMode::getDesktopMode());
	desk_VM.height -= 100;
	desk_VM.width -= 10;
	if (win_size.y > desk_VM.height) {
		float scale = (float)desk_VM.height/(float)win_size.y;

		field.setDrawScale(scale);
		win_size.x = width*field.getDrawTileSize();
		win_size.y = height*field.getDrawTileSize();
	}
	if (win_size.x > desk_VM.width) {
		float scale = (float)desk_VM.width/(float)win_size.x;

		field.setDrawScale(scale);
		win_size.x = width*field.getDrawTileSize();
		win_size.y = height*field.getDrawTileSize();
	}
	win.close();
	VMode.width = win_size.x;
	VMode.height = win_size.y;
	win.create(VMode, "Minesweeper - SFML");
	win.setFramerateLimit(30);
}
