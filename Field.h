/****************************************************************************************
	created:	2013/02/16   22:49
	filename: 	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test\Field.h
	file path:	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test
	file base:	Field
	file ext:	h
	author:		dVb
	
	purpose:	Class that is able to show a field with a couple of rules. You have to
				set following variables to let it work properly:
				  - sf::Vector2u size			using		Field::setFieldSize()
				  - int no_bombs				using		Field::setNoBombs()
				  - sf::Texture tex				using		Field::setTexture()
				  - unsigned int tex_size		using		Field::setTextureTileSize()
				  - sf::RenderWindow *app		using		Field::setWindow()

				Next you need to call Field::create() which will then initialize the
				the field using the variables you inserted.
				Use Field::update() in every loop to update the textures of each tile
				and finally use Field::draw() to draw the funcion to you window.


				Good to know:
				  1) draw size of the tiles are defaulted to the texture tile size
				  2) you can create a new random field using Field::reset()
				  3) get useful debug info with Field::setDebugState()
				  4) definitions found in file:Filed.cpp
****************************************************************************************/

#include "stdafx.h"

#pragma once

class Field {

public:
	enum state {UNINITIALIZED, INITIALIZED};

	// CONSTRUCTORS
	Field(bool =false);							// sets default values for field ~ Field(10,10,20,false)
	Field(int x, int y, int b, bool =false);	// (x,y) = dimensions of field; b = number of bombs on field
	Field(const Field&);						// copy constructor
	~Field();									// destructor



	// SET FUNTCIONS
	void setFieldSize(sf::Vector2u);	// change dimensions of field
	void setFieldSize(uint,uint);		//   ^
	void setNoBombs(uint);				// change number of bombs on field (does not add them)

	void setTexture(string);			// set the texture file
	void setTextureTileSize(uint);		// set size of texture tiles
	void setDrawTileSize(uint);			// set size of drawing tiles (default to size of texture tiles)
	void setDrawScale(float);			// set size of drawing tiles using relative scale (overwrites setDrawTileSize)
	void setWindow(sf::RenderWindow*);	// sets pointer to renderwindow used in draw function

	void setDebugState(bool);			// sets debug flag
	void setAllRevealState(bool);		// will set all tiles' debug states when debug is enabled
	void setRevealState(uint,bool,bool =true);
	void setRevealState(uint,uint,bool,bool =true);	// sets reveal state of tile (x,y) when debug is enabled
	void changeFlag(uint);
	void changeFlag(uint,uint);
	void setPressedState(uint,bool);
	void setAllPressedState(bool);


	// FIELD FUNCTIONS
	void create();						// initializes field: adds bombs, calculates numbers, initializes draw function
	void create(uint x,uint y,uint);	//   ^ with extra options
	void update(sf::Event);				// updates texture coordinates
	void reset();						// clears and recreates field
	void reset(uint x, uint y,uint);	//   ^ with extra options

	void updateVertex(uint,bool endgame =false);
	void updateVertex(uint,uint,bool endgame =false);
	void updateAllVertex(bool endgame =false, bool force =false);

	void giveHint();

	bool isWon();
	bool isLost();



	// GET FUNCTIONS
	sf::Vector2u getFieldSize() const;
	unsigned int getNoBombs() const;

	sf::Texture*      getTexture();
	unsigned int      getTextureTileSize() const;
	unsigned int      getDrawTileSize() const;
	float             getDrawScale() const;
	sf::RenderWindow* getWindow() const;

	bool getDebugState() const; 
	bool getRevealState(sf::Vector2u) const;
	bool getRevealState(uint,uint) const;
	void getNeighbours(uint, uint *out);



	// OUTPUT
	void coutField(); // prints field on cout
	void draw();      // draws field on window




private:
	// PRIVATE CLASSES & STRUCS
	void addBombs();			// adds bombs to the correct value (does not remove)
	void initNumbers();			// recalculates numbers
	void initVertex();			// initialized vertex array
	void makeSpecial(int);
	void makeSpecial(int,int);
	bool makeRandomSpecial();

	void clear();				// sets all tiles to Tile::UNINITIALIZED
	void clear(int x, int y);   //   ^ with extra options

	struct Tile { // holds properties of a tile
		bool is_revealed;
		bool is_flagged;
		bool is_pressed;
		bool is_special;
		bool update_vertex;
		enum tiletype {BOMB=-1, EMPTY, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, UNINITIALIZED};
		tiletype type;
	};


	// MEMBER VARIABLES
	state m_state;
	sf::Vector2u size;
	unsigned int no_bombs, no_flags;

	sf::Texture tex;
	unsigned int tex_size, draw_size;
	sf::RenderWindow *win;

	vector<Tile> field;
	sf::VertexArray VA;

	bool is_won, is_lost;
	bool is_debugging;
};