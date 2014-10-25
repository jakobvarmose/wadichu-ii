// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QList>

class GameModel {
public:
	GameModel ();
	void update ();
	void fire (QSet<int> &res, int x, int y, int xi, int yi, int length);
	class Bomb {
	public:
		int x, y;
		int length;
		int count;
		Bomb () : x (0), y (0), count (0), length (2) {
		}
		Bomb (int x, int y, int count) : x (x), y (y), count (count), length (2) {
		}
	};
	class Player {
	public:
		int x, y;
		bool keyUp, keyLeft, keyDown, keyRight;
		bool keyBomb;
		int bombCount;
		Player () :
			x (0), y (0),
			keyUp (false), keyLeft (false),
			keyDown (false), keyRight (false),
			keyBomb (false), bombCount (10)
		{
		}
		void update (GameModel *game);
	};
	class Fire {
	public:
		int x, y;
	};
	class Tile {
	public:
		int type;
		bool fire;
		bool bomb;
		Tile () : type (3), fire (false), bomb (false) {
		}
		bool empty () {
			return this->type == 0 && this->bomb == false;
		}
		bool solid () {
			return this->type == 3;
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
	QList <Bomb> bombs;
	QList <Player> players;
	QList <Fire *> fires;
	QList <Monster *> monsters;
};
