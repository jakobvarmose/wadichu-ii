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
void GameModel::start () {
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			if (
				x == 0 ||
				x == 18 ||
				y == 0 ||
				y == 18 ||
				(x % 2 == 0 && y % 2 == 0)
			) {
				this->tiles [x] [y].type = 3;
				this->tiles [x] [y].item = ITEM_NONE;
			} else if (
				(x <= 2 || x >= 16) &&
				(y <= 2 || y >= 16)
			) {
				this->tiles [x] [y].type = 0;
				this->tiles [x] [y].item = ITEM_NONE;
			} else {
				if (rand () % 1000 < 700) {
					this->tiles [x] [y].type = 0;
					this->tiles [x] [y].item = ITEM_NONE;
				} else {
					this->tiles [x] [y].type = 1;
					int q = rand () % 1000;
					if (q < 200) {
						this->tiles [x] [y].item = ITEM_PLUSBOMB;
					} else if (q < 400) {
						this->tiles [x] [y].item = ITEM_PLUSLENGTH;
					} else {
						this->tiles [x] [y].item = ITEM_NONE;
					}
				}
			}
		}
	}
	for (int i = 0; i < 10; ++i) {
		this->monsters.push_back (new Monster (64 * i + 32, 32));
	}
}
void GameModel::update () {
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			Tile &tile = this->tiles [x] [y];
			if (tile.fire) {
				tile.fireCount--;
				if (tile.fireCount == 0) {
					tile.fire = false;
					tile.type = 0;
				}
			}
		}
	}
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			Tile &tile = this->tiles [x] [y];
			if (tile.bomb) {
				tile.bombCount -= 1;
				if (tile.bombCount == 0) {
					tile.bomb = false;
					tile.player->bombCount++;
					QSet<int> res;
					res.insert (100 * x + y);
					fire (res, x, y, 1, 0, tile.bombLength);
					fire (res, x, y, -1, 0, tile.bombLength);
					fire (res, x, y, 0, 1, tile.bombLength);
					fire (res, x, y, 0, -1, tile.bombLength);
					for (int i : res) {
						int x = i / 100;
						int y = i % 100;
						this->tiles [x] [y].fire = true;
						this->tiles [x] [y].fireCount = 25;
					}
					tile.bomb = false;
				}
			}
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
		Tile &tile = this->tiles [x] [y];
		if (!tile.solid ()) {
			res.insert (100 * x + y);
		}
		if (tile.bomb) {
			tile.bomb = false;
			tile.player->bombCount++;
			fire (res, x, y, 1, 0, tile.bombLength);
			fire (res, x, y, -1, 0, tile.bombLength);
			fire (res, x, y, 0, 1, tile.bombLength);
			fire (res, x, y, 0, -1, tile.bombLength);
		}
	}
}
void GameModel::Player::update (GameModel *game) {
	int speed = 4;
	int dx = this->keyRight - this->keyLeft;
	int dy = this->keyUp - this->keyDown;
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
	Tile &tile = game->tiles [x] [y];
	if (this->keyBomb && this->bombCount > 0 && tile.empty ()) {
		this->bombCount--;
		tile.bomb = true;
		tile.bombCount = 100;
		tile.bombLength = this->bombLength;
		tile.player = this;
	}
	if (tile.item == ITEM_PLUSBOMB) {
		this->bombCount++;
		tile.item = ITEM_NONE;
	} else if (tile.item == ITEM_PLUSLENGTH) {
		this->bombLength++;
		tile.item = ITEM_NONE;
	}
}
