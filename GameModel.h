// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QList>


enum class TypeEnum {
	Empty,
	Solid,
	Statue,
	Immune
};

enum class ItemEnum {
	None,
	PlusBomb,
	PlusLength,
	PlusLife
};

enum class MonsterEnum {
	Cr = 1,
	Fish,
	Lithor,
	Fireball
};

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
		int lives;
		Player () :
			x (0), y (0),
			keyUp (false), keyLeft (false),
			keyDown (false), keyRight (false),
			keyBomb (false), bombCount (1), bombLength (2), lives (0)
		{
		}
		void update (GameModel *game);
	};
	class Tile {
	public:
		TypeEnum type;
		ItemEnum item;
		bool fire;
		bool bomb;
		int bombCount;
		int fireCount;
		int bombLength;
		Player *player;
		Tile () : type (TypeEnum::Immune), item (ItemEnum::None), fire (false), bomb (false) {
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
		int xi, yi;
		int shootingTimer;
		int shootingDir;
		Monster (MonsterEnum type, int x, int y) : alive (true), type (type), x (x), y (y), xi (0), yi (0), shootingDir (0) {
		}
		void update (GameModel &model);
	};
	Tile tiles [19] [19];
	QList <Player> players;
	QList <Monster *> monsters;
};

struct Level {
	struct Tile {
		quint8 type;
		quint8 item;
	};
	struct Monster {
		quint8 type;
		quint8 x;
		quint8 y;
	};
	Tile tiles [19] [19];
	QList <Monster> monsters;
};

class LevelSet {
public:
	QList <Level> levels;
	quint16 version;
	LevelSet (const QByteArray &data);
	QByteArray serialize () const;
};
