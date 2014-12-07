// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QList>
#include "Enum.h"

class Level;

class GameModel {
public:
	GameModel ();
	void start ();
	void load (const Level &level);
	void update ();
	void fire (QSet<int> &res, int x, int y, int xi, int yi, int length);
	class Player {
	public:
		int x, y;
		bool keyUp, keyLeft, keyDown, keyRight;
		bool keyBomb;
		int bombCount;
		int bombLength;
		int lives;
		int immuneCount;
		Player () :
			x (0), y (0),
			keyUp (false), keyLeft (false),
			keyDown (false), keyRight (false),
			keyBomb (false), bombCount (1), bombLength (2), lives (0),
			immuneCount (0)
		{
		}
		void update (GameModel *game);
		void hit ();
	};
	class Tile {
	public:
		TypeEnum type;
		ItemEnum item;
		bool bomb;
		int bombCount;
		int fireCount;
		int bombLength;
		Player *player;
		Tile () : type (TypeEnum::Immune), item (ItemEnum::None), bomb (false), fireCount (0) {
		}
		bool empty () const {
			return this->type == TypeEnum::Empty && this->bomb == false;
		}
		bool solid () const {
			return this->type == TypeEnum::Immune;
		}
		void update () {
		}
	};
	class Monster {
	public:
		bool alive;
		MonsterEnum type;
		int x, y;
		int w, h;
		int xi, yi;
		int shootingDir;
		int shootingTimer;
		Monster (MonsterEnum type, int x, int y, int w = 32, int h = 32) : alive (true), type (type), x (x), y (y), w (w), h (h), xi (0), yi (0), shootingDir (0), shootingTimer (0) {
		}
		void update (GameModel &model);
	};
	Tile tiles [19] [19];
	QList <Player> players;
	QList <Monster *> monsters;
};
