// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QList>

#define TYPE_EMPTY      0
#define TYPE_SOLID      1
#define TYPE_IMMUNE     3

#define ITEM_NONE       0
#define ITEM_PLUSBOMB   1
#define ITEM_PLUSLENGTH 2

class GameModel {
public:
	GameModel ();
	void start ();
	void update ();
	void fire (QSet<int> &res, int x, int y, int xi, int yi, int length);
	class Player {
	public:
		int x, y;
		bool keyUp, keyLeft, keyDown, keyRight;
		bool keyBomb;
		int bombCount;
		int bombLength;
		Player () :
			x (0), y (0),
			keyUp (false), keyLeft (false),
			keyDown (false), keyRight (false),
			keyBomb (false), bombCount (1), bombLength (2)
		{
		}
		void update (GameModel *game);
	};
	class Tile {
	public:
		int type;
		int item;
		bool fire;
		bool bomb;
		int bombCount;
		int fireCount;
		int bombLength;
		Player *player;
		Tile () : type (3), item (0), fire (false), bomb (false) {
		}
		bool empty () const {
			return this->type == 0 && this->bomb == false;
		}
		bool solid () const {
			return this->type == 3;
		}
		void update () {
			if (this->bombCount) {
				--bombCount;
				if (!this->bombCount) {
					
				}
			}
		}
	};
	class Monster {
	public:
		int x, y;
		int type;
		Monster (int x, int y) : x (x), y (y) {
		}
	};
	Tile tiles [19] [19];
	QList <Player> players;
	QList <Monster *> monsters;
};
