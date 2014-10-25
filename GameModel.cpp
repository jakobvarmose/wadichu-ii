// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#include "GameModel.h"

#include <QSet>

GameModel::GameModel () {
	this->players.clear ();
	this->players.append (GameModel::Player ());
	this->players.append (GameModel::Player ());
	this->players [0].x = 1 * 32;
	this->players [0].y = 17 * 32;
	this->players [1].x = 17 * 32;
	this->players [1].y = 1 * 32;
}
void GameModel::update () {
	for (int i = 0; i != this->bombs.size (); ++i) {
		this->bombs [i].count -= 1;
		if (this->bombs [i].count == 0) {
			this->tiles [
				this->bombs [i].x / 32
			] [
				this->bombs [i].y / 32
			].bomb = false;
			QSet<int> res;
			res.insert (
				100 * (this->bombs [i].x / 32) +
				(this->bombs [i].y / 32)
			);
			fire (
				res,
				this->bombs [i].x / 32,
				this->bombs [i].y / 32,
				1, 0,
				this->bombs [i].length
			);
			fire (
				res,
				this->bombs [i].x / 32,
				this->bombs [i].y / 32,
				-1, 0,
				this->bombs [i].length
			);
			fire (
				res,
				this->bombs [i].x / 32,
				this->bombs [i].y / 32,
				0, 1,
				this->bombs [i].length
			);
			fire (
				res,
				this->bombs [i].x / 32,
				this->bombs [i].y / 32,
				0, -1,
				this->bombs [i].length
			);
			for (int i : res) {
				int x = i / 100;
				int y = i % 100;
				this->tiles [x] [y].fire = true;
			}
			this->bombs.removeAt (i);
			--i;
		}
	}
	for (int i = 0; i != this->players.size (); ++i) {
		this->players [i].update (this);
	}
}
void GameModel::fire (QSet<int> &res, int x, int y, int xi, int yi, int length) {
	for (int i = 0; i < length && this->tiles [x] [y].empty (); ++i) {
		x += xi;
		y += yi;
		if (!this->tiles [x] [y].solid ()) {
			res.insert (100 * x + y);
		}
	}
}
void GameModel::Player::update (GameModel *game) {
	int speed = 4;
	int dx = this->m_keyRight - this->m_keyLeft;
	int dy = this->m_keyUp - this->m_keyDown;
	if (dx != 0) {
		if (this->x % 32 == 0) {
			int x = this->x / 32 + dx;
			bool a = game->tiles [x] [this->y / 32].empty ();
			bool b = game->tiles [x] [(this->y + 31) / 32].empty ();
			if (a && b) {
				this->x += dx * speed;
			} else if (a) {
				this->y += -1 * speed;
			} else if (b) {
				this->y += 1 * speed;
			}
		} else {
			this->x += dx * speed;
		}
	}
	if (dy != 0) {
		if (this->y % 32 == 0) {
			int y = this->y / 32 + dy;
			bool a = game->tiles [this->x / 32] [y].empty ();
			bool b = game->tiles [(this->x + 31) / 32] [y].empty ();
			if (a && b) {
				this->y += dy * speed;
			} else if (a) {
				this->x += -1 * speed;
			} else if (b) {
				this->x += 1 * speed;
			}
		} else {
			this->y += dy * speed;
		}
	}
	int x = (this->x + 16) / 32;
	int y = (this->y + 16) / 32;
	if (this->m_keyBomb && this->m_bombCount > 0 && game->tiles [x] [y].empty ()) {
		this->m_bombCount -= 1;
		game->tiles [x] [y].bomb = true;
		game->bombs.append (GameModel::Bomb (
			(this->x + 16) / 32 * 32,
			(this->y + 16) / 32 * 32,
			100
		));
	}
}
