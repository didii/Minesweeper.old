/********************************************************************
	created:	2013/02/16   23:10
	filename: 	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test\Field.cpp
	file path:	C:\Users\Dieter\documents\visual studio 2010\Projects\Test\Test
	file base:	Field
	file ext:	cpp
	author:		dVb
	
	purpose:	Definitions of File:Field.h.
*********************************************************************/

#include "stdafx.h"

#pragma once
#include "Field.h"
#include "Game.h"

// CONSTRUCTORS
Field::Field(bool bug) : is_debugging(bug) {
	setFieldSize(10,10);
	setNoBombs(20);
	clear(10,10);
}
Field::Field(int w,int h,int b, bool bug) : is_debugging(bug) {
	setFieldSize(w,h);
	setNoBombs(b);
	clear(w,h);
}
Field::Field(const Field &copy) {
	this->size.x = copy.size.x;
	this->size.y = copy.size.y;
	this->tex_size = copy.tex_size;
	this->draw_size = copy.draw_size;

	for (unsigned int i = 0; i < field.size(); ++i) {
		this->field[i] = copy.field[i];
	}
}
Field::~Field() {
	if (is_debugging)
		cout << "Field was destructed!\n\n\n";
}



// SET FUNCTIONS
void Field::setFieldSize(sf::Vector2u s) {
	setFieldSize(s.x, s.y);
}
void Field::setFieldSize(unsigned int x, unsigned int y) {
	size.x = x;
	size.y = y;
	field.resize(size.x*size.y);
	VA.resize(size.x*size.y);
	if (is_debugging)
		cout << "Field size set to (" << size.x << ", " << size.y << ")\n";
}
void Field::setNoBombs(unsigned int b) {
	no_bombs = b;
	if (is_debugging)
		cout << "#bombs set to " << no_bombs << endl;
}

void Field::setTexture(string file) {
	tex.loadFromFile(file);
	if (is_debugging)
		cout << "Texture loaded from " << file << endl;
}
void Field::setTextureTileSize(unsigned int s) {
	tex_size = s;
	if (is_debugging)
		cout << "Texture's tile size set to " << tex_size << endl;
}
void Field::setDrawTileSize(unsigned int s) {
	draw_size = s;
	if (is_debugging)
		cout << "Tile's drawing size set to " << draw_size << endl;
}
void Field::setDrawScale(float scale) {
	setDrawTileSize((int)((float)tex_size*scale));
}
void Field::setWindow(sf::RenderWindow* w) {
	win = w;
	if (is_debugging)
		cout << "Window is linked\n";
}

void Field::setDebugState(bool state) {
	is_debugging = state;
}
void Field::setAllRevealState(bool state) {
	bool was_debugging = is_debugging;
	is_debugging = false;
	for (uint i = 0; i < field.size(); ++i) {
		setRevealState(i,state,false);
	}
	is_debugging = was_debugging;
	if (is_debugging)
		cout << "All tiles are set to revealed\n";
}
void Field::setRevealState(unsigned int i, bool state, bool check_neighbours) {
	if (i < 0 || i > field.size()) {
		cout << "Error in setRevealState(): Argument out of bounds\n";
		return;
	}
	field[i].is_revealed = state;
	field[i].update_vertex = true;
	if (is_debugging)
		cout << "Tile (" << i%size.x << ", " << (int)((float)(i/size.x)) << ") = " << i << " is set to revealed\n";
	if (!check_neighbours) {
		return;
	}
	if (field[i].type == Tile::EMPTY) {
		uint pos[9];
		getNeighbours(i, pos);
		for (uint j = 0; j < 9; ++j) {
			if (!field[pos[j]].update_vertex) {
				setRevealState(pos[j],state);
			}
		}
	}
}
void Field::setRevealState(unsigned int x, unsigned int y, bool state, bool check_neighbours) {
	setRevealState(y*size.x + x, state, check_neighbours);
}
void Field::changeFlag(unsigned int i) {
	if (!field[i].is_revealed) {
		if (field[i].is_flagged)
			no_flags--;
		else
			no_flags++;
		cout << "Bombs left: " << no_bombs - no_flags << endl;
		field[i].is_flagged = !field[i].is_flagged;
		field[i].update_vertex = true;

		if (is_debugging)
			cout << "Tile (" << i%size.x << ", " << (int)((float)(i/size.x)) << ") = " << i << " has been (un)flagged\n";
	}
}
void Field::setPressedState(unsigned int i, bool state) {
	if (field[i].is_pressed != state) {
		field[i].is_pressed = state;
		field[i].update_vertex = true;
		if (is_debugging)
			cout << "Tile (" << i%size.x << ", " << (int)((float)(i/size.x)) << ") = " << i << " pressed state set to " << state << endl;
	}
}
void Field::setAllPressedState(bool state) {
	bool was_debugging = is_debugging;
	is_debugging = false;
	for (uint i = 0; i < field.size(); ++i) {
		setPressedState(i,state);
	}
	is_debugging = was_debugging;
	if (is_debugging)
		cout << "All tiles pressed state set to " << state << endl;
}



// FIELD FUNCTIONS
void Field::create() {
	if (is_debugging)
		cout << "Calling Field::create()\n";
	addBombs();
	initNumbers();
	VA = sf::VertexArray(sf::Quads, 4*size.x*size.y);
	initVertex();
	makeRandomSpecial();
	m_state = INITIALIZED;
	cout << "Bombs left: " << no_bombs << endl;
}
void Field::create(unsigned int x,unsigned int y,unsigned int b) {
	setFieldSize(x,y);
	setNoBombs(b);
	create();
}
void Field::update(sf::Event ev) {
	sf::Vector2i pos_mouse(sf::Mouse::getPosition(*win));
	int x,y;
	x = (int)((float)pos_mouse.x/(float)draw_size);
	y = (int)((float)pos_mouse.y/(float)draw_size);
	if (x < 0 || x > (int)size.x || y < 0 || y > (int)size.y) {
		return;
	}
	int i = y*size.x+x;
	if (ev.type == sf::Event::MouseButtonPressed && ev.key.code == sf::Mouse::Left) {
		if (!field[i].is_revealed)
			setPressedState(i,true);
	}

	if (ev.type == sf::Event::MouseButtonReleased) {
		if (ev.key.code == sf::Mouse::Left) {
			if (field[i].is_pressed && !field[i].is_flagged && !field[i].is_revealed) {
				setRevealState(i,true);

				// check win/lose state
				if (field[i].type == Tile::BOMB)
					is_lost = true;
			}
			setAllPressedState(false);
		}
		if (ev.key.code == sf::Mouse::Right) {
			if (!field[i].is_revealed) {
				changeFlag(i);
			}
		}
	}
	updateAllVertex();
}
void Field::reset() {
	if (is_debugging)
		cout << "Field will reset\n";
	clear();
	create();
}
void Field::reset(unsigned int x, unsigned int y, unsigned int b) {
	setFieldSize(x, y);
	setNoBombs(b);
	reset();
}

void Field::updateVertex(unsigned int i, bool endgame) {
	if (i < 0 || i > field.size()) {
		cout << "Error using updateVertex(): arguments out of bounds\n";
		return;
	}
	if (field[i].is_flagged) {
		if (!endgame) {
			VA[4*i].texCoords   = sf::Vector2f((float)(2*tex_size), 0.f);
			VA[4*i+1].texCoords = sf::Vector2f((float)(3*tex_size), 0.f);
			VA[4*i+2].texCoords = sf::Vector2f((float)(3*tex_size), (float)tex_size);
			VA[4*i+3].texCoords = sf::Vector2f((float)(2*tex_size), (float)tex_size);
		} else {
			if (field[i].type == Tile::BOMB) {
				VA[4*i].texCoords   = sf::Vector2f((float)(2*tex_size), 0.f);
				VA[4*i+1].texCoords = sf::Vector2f((float)(3*tex_size), 0.f);
				VA[4*i+2].texCoords = sf::Vector2f((float)(3*tex_size), (float)tex_size);
				VA[4*i+3].texCoords = sf::Vector2f((float)(2*tex_size), (float)tex_size);
			} else {
				VA[4*i].texCoords   = sf::Vector2f((float)(15*tex_size), 0.f);
				VA[4*i+1].texCoords = sf::Vector2f((float)(16*tex_size), 0.f);
				VA[4*i+2].texCoords = sf::Vector2f((float)(16*tex_size), (float)tex_size);
				VA[4*i+3].texCoords = sf::Vector2f((float)(15*tex_size), (float)tex_size);
			}
		}
		return;
	}
	if (field[i].is_revealed) {
		int type = field[i].type;
		VA[4*i].texCoords   = sf::Vector2f((float)(tex_size*(4+(int)type)), 0.f);
		VA[4*i+1].texCoords = sf::Vector2f((float)(tex_size*(5+(int)type)), 0.f);
		VA[4*i+2].texCoords = sf::Vector2f((float)(tex_size*(5+(int)type)), (float)tex_size);
		VA[4*i+3].texCoords = sf::Vector2f((float)(tex_size*(4+(int)type)), (float)tex_size);
	} else {
		if (field[i].is_pressed) {
			VA[4*i].texCoords   = sf::Vector2f( (float)tex_size,   0.f             );
			VA[4*i+1].texCoords = sf::Vector2f( 2*(float)tex_size, 0.f             );
			VA[4*i+2].texCoords = sf::Vector2f( 2*(float)tex_size, (float)tex_size );
			VA[4*i+3].texCoords = sf::Vector2f( (float)tex_size,   (float)tex_size );
		} else {
			VA[4*i].texCoords   = sf::Vector2f( 0.f,             0.f             );
			VA[4*i+1].texCoords = sf::Vector2f( (float)tex_size, 0.f             );
			VA[4*i+2].texCoords = sf::Vector2f( (float)tex_size, (float)tex_size );
			VA[4*i+3].texCoords = sf::Vector2f( 0.f,             (float)tex_size );
		}
	}
}
void Field::updateVertex(unsigned int x,unsigned int y, bool endgame) {
	updateVertex(y*size.y+x, endgame);
}
void Field::updateAllVertex(bool endgame, bool force) {
	bool was_debugging = is_debugging;
	is_debugging = false;
	unsigned int no_updates = 0;
	for (uint i = 0; i < field.size(); ++i) {
		if (field[i].update_vertex || force) {
			updateVertex(i,endgame);
			field[i].update_vertex = false;
			no_updates++;
		}
	}
	is_debugging = was_debugging;
	if (is_debugging && no_updates > 0) {
		if (no_updates == field.size())
			cout << "All";
		else
			cout << no_updates;
		cout << " vertex(es) updated (";
		if (force)
			cout << "forced";
		else
			cout << "not forced";
		cout << ")\n";
	}
}

void Field::giveHint() {
	if (makeRandomSpecial())
		return;
	int r = rand() % field.size();
	for (;; r = rand() % field.size()) {
		if (field[r].type != Tile::BOMB && !field[r].is_revealed)
			break;
	}
	makeSpecial(r);
}

bool Field::isWon() {
	if (is_won)
		return true;
	unsigned int nr_revealed = 0;
	for (uint i = 0; i < field.size(); ++i)
		nr_revealed += field[i].is_revealed;
	if (nr_revealed == field.size() - no_bombs)
		return (is_won = true);
	return false;
}
bool Field::isLost() {
	return is_lost;
}


// GET FUNCTIONS
sf::Vector2u Field::getFieldSize() const {
	return size;
}
unsigned int Field::getNoBombs() const {
	return no_bombs;
}

sf::Texture* Field::getTexture() {
	return &tex;
}
unsigned int Field::getTextureTileSize() const {
	return tex_size;
}
unsigned int Field::getDrawTileSize() const {
	return draw_size;
}
float        Field::getDrawScale() const {
	return (float)(draw_size/tex_size);
}
sf::RenderWindow* Field::getWindow() const {
	return win;
}

bool Field::getDebugState() const {
	return is_debugging;
}
bool Field::getRevealState(sf::Vector2u pos) const {
	return getRevealState(pos.x,pos.y);
}
bool Field::getRevealState(unsigned int x,unsigned  int y) const {
	if (x < 0 || x > size.x || y < 0 || y > size.y) {
		cout << "Error in getRevealState(): position out of bounds. Returning false\n";
		return false;
	}
	return field[y*size.y+x].is_revealed;
}
void Field::getNeighbours(unsigned int i, unsigned int *out) {
	for (uint j = 0; j < 9; ++j) {
		int p = i + ((int)((float)j/3.f)-1)*size.x + (j%3-1);
		unsigned int temp = abs((int)(p%size.x-i%size.x));
		if ( p < 0 || p >= (int)field.size() || temp == size.x+1 || temp == size.x-1 )
			p = i;
		*(out+j) = p;
	}
}


// OUTPUT FUNCTIONS
void Field::coutField() {
	cout << "  |";
	for (unsigned int i = 0; i < size.x; ++i) {
		if (i < 10)
			cout << i << " ";
		else
			cout << i;
	}
	cout << "\n--+";
	for (unsigned int i = 0; i < size.x; ++i) {
		cout << "--";
	}
	cout << "-";
	for (unsigned int i = 0; i < field.size(); ++i) {
		if (i%size.x == 0) {
			cout << endl;
			if (i < 10*size.x)
				cout << " " << i/size.x << "|";
			else
				cout << i/size.x << "|";
		}
		if (field[i].type == Tile::BOMB)
			cout << "* ";
		else
			cout << (int)field[i].type << " ";
	}
	cout << endl;
}
void Field::draw() {
	win->draw(VA, &tex);
}



// PRIVATE FUNCTIONS
void Field::addBombs() {
	if (no_bombs > size.x*size.y) {
		cout << "Error adding bombs: no_bombs exceeds field dimensions (" << no_bombs << ")\n";
		return;
	}
	
	unsigned int bombs_on_field = 0;
	for (unsigned int i = 0; i < field.size(); ++i) {
		if (field[i].type == Tile::BOMB) {
			bombs_on_field+=1;
		}
	}
	if (is_debugging) {
		cout << "Currently " << bombs_on_field << " bombs on field\nAdding " << no_bombs - bombs_on_field << " bombs.\n";
	}
	while (bombs_on_field < no_bombs) {
		int r = rand() % (size.y*size.x);
		if (field[r].type != Tile::BOMB) {
			field[r].type = Tile::BOMB;
			bombs_on_field+=1;
			if (is_debugging) {
				cout << "Bomb added (" << r%size.x << ", " << static_cast<int>(r/size.x) << ")\n";
			}
		}
	}
}
void Field::initNumbers() {
	for (unsigned int i = 0; i < field.size(); ++i) {
		// if current tile is bomb, skip
		if (field[i].type == Tile::BOMB) {
			continue;
		}
		field[i].type = Tile::EMPTY;
		// create array pointing to locations to check for bombs
		int check_pos[8] = {i-size.x-1, i-size.x, i-size.x+1,
							  i-1,				  i+1,
							i+size.x-1, i+size.x, i+size.x+1};
		if ( i%size.x == 0 ) {
			check_pos[0] = i;
			check_pos[3] = i;
			check_pos[5] = i;
		}
		if ( i%size.x == size.x-1 ) {
			check_pos[2] = i;
			check_pos[4] = i;
			check_pos[7] = i;
		}
		// loop over array
		for (unsigned int j = 0; j < 8; ++j) {
			if (check_pos[j] >= 0 && check_pos[j] < (int)field.size()) {
				if (field[check_pos[j]].type == Tile::BOMB) {
					field[i].type = (Tile::tiletype)(field[i].type + 1);
				}
			}
		}
	}
	if (is_debugging) {
		cout << "Numbers are initialized:\n";
		coutField();
		cout << endl;
	}
}
void Field::initVertex() {
	for (unsigned int i = 0; i < size.x*size.y; ++i) {
		VA[4*i].texCoords   = sf::Vector2f( 0.f,             0.f             );
		VA[4*i+1].texCoords = sf::Vector2f( (float)tex_size, 0.f             );
		VA[4*i+2].texCoords = sf::Vector2f( (float)tex_size, (float)tex_size );
		VA[4*i+3].texCoords = sf::Vector2f( 0.f,             (float)tex_size );

		VA[4*i].position   = sf::Vector2f( (float)(draw_size*(i%size.x)),     (float)(draw_size*(int)(i/size.x))   );
		VA[4*i+1].position = sf::Vector2f( (float)(draw_size*((i%size.x)+1)), (float)(draw_size*(int)(i/size.x))   );
		VA[4*i+2].position = sf::Vector2f( (float)(draw_size*((i%size.x)+1)), (float)(draw_size*(int)(i/size.x+1)) );
		VA[4*i+3].position = sf::Vector2f( (float)(draw_size*(i%size.x)),     (float)(draw_size*(int)(i/size.x+1)) );
	}
	if (is_debugging)
		cout << "VertexArray has been initialized/reset\n";
}
void Field::makeSpecial(int r){
	field[r].is_special = true;
	VA[4*r].texCoords   = sf::Vector2f(14.f*(float)tex_size, 0.f);
	VA[4*r+1].texCoords = sf::Vector2f(15.f*(float)tex_size, 0.f);
	VA[4*r+2].texCoords = sf::Vector2f(15.f*(float)tex_size, (float)tex_size);
	VA[4*r+3].texCoords = sf::Vector2f(14.f*(float)tex_size, (float)tex_size);
	field[r].update_vertex = false;
	if (is_debugging)
		cout << "Tile (" << r%size.x << ", " << (int)((float)(r/size.x)) << ") = " << r << " was made special\n";
}
void Field::makeSpecial(int x, int y) {
	makeSpecial(y*size.x + x);
}
bool Field::makeRandomSpecial() {
	int nr = field.size();
	for (uint i = 0; i < field.size(); ++i) {
		if (field[i].type != Tile::EMPTY || field[i].is_revealed)
			nr--;
	}
	if (nr == 0) {
		if (is_debugging) {
			cout << "No special block this time :(\n";
		}
		return false;
	}
	int r = rand() % field.size();
	for (;; r = rand() % field.size()) {
		if (field[r].type == Tile::EMPTY && !field[r].is_revealed)
			break;
	}
	makeSpecial(r);
	return true;
}
void Field::clear() {
	m_state = UNINITIALIZED;
	for (uint i = 0; i < field.size(); ++i) {
		field[i].is_revealed = false;
		field[i].is_flagged = false;
		field[i].update_vertex = true;
		field[i].type = Tile::UNINITIALIZED;
	}
	is_won = false;
	is_lost = false;
	no_flags = 0;
	if (is_debugging) {
		cout << "Cleared field\n";
	}
}
void Field::clear(int x, int y) {
	setFieldSize(x,y);
	clear();
}