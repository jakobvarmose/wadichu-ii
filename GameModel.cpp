// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#include "GameModel.h"

#include <QSet>
#include "LevelSet.h"

GameModel::GameModel () {
	this->players.clear ();
	this->players.append (Player ());
	this->players.append (Player ());
	this->players [0].x = 1 * 32;
	this->players [0].y = 17 * 32;
	this->players [1].x = 17 * 32;
	this->players [1].y = 1 * 32;
}
void GameModel::load (const Level &level) {
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			this->tiles [x] [y] = Tile ();
			this->tiles [x] [y].type = level.tiles [x] [y].type;
			this->tiles [x] [y].item = level.tiles [x] [y].item;
		}
	}
	for (Monster *monster : this->monsters) {
		delete monster;
	}
	this->monsters.clear ();
	for (const Level::Monster &monster : level.monsters) {
		Monster *m = new Monster (monster.type, 32 * monster.x, 32 * monster.y);
		this->monsters.append (m);
	}
	this->players [0].x = 1 * 32;
	this->players [0].y = 17 * 32;
	this->players [1].x = 17 * 32;
	this->players [1].y = 1 * 32;
}
void GameModel::start () {
	Level level;
	level.random ();
	this->load (level);
}
void GameModel::update () {
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			Tile &tile = this->tiles [x] [y];
			if (tile.fireCount) {
				tile.fireCount--;
				if (tile.fireCount == 0) {
					tile.type = TypeEnum::Empty;
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
						this->tiles [x] [y].fireCount = 25;
					}
					tile.bomb = false;
				}
			}
		}
	}
	for (int i = 0; i < this->monsters.size (); ++i) {
		this->monsters [i]->update (*this);
		if (!this->monsters [i]->alive && !this->tiles [(this->monsters [i]->x + 16) / 32] [(this->monsters [i]->y + 16) / 32].fireCount) {
			this->monsters.removeAt (i--);
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
void GameModel::Monster::update (GameModel &game) {
	if (this->alive) {
		switch (this->type) {
		case MonsterEnum::Cr:
		case MonsterEnum::Fish:
		case MonsterEnum::Lithor:
			if (this->shootingTimer == 0) {
				if (this->x % 32 == 0 && this->y % 32 == 0) {
					int x = this->x / 32;
					int y = this->y / 32;
					QList <QPair <int, int>> dirs;
					const Player *player = this->closestPlayer (game);
					if (player) {
						if (player->x > this->x && game.tiles [x + 1] [y].empty ()) {
							dirs.append (qMakePair (1, 0));
						}
						if (player->x < this->x && game.tiles [x - 1] [y].empty ()) {
							dirs.append (qMakePair (-1, 0));
						}
						if (player->y > this->y && game.tiles [x] [y + 1].empty ()) {
							dirs.append (qMakePair (0, 1));
						}
						if (player->y < this->y && game.tiles [x] [y - 1].empty ()) {
							dirs.append (qMakePair (0, -1));
						}
					}
					if (dirs.isEmpty ()) {
						if (xi >= 0 && game.tiles [x + 1] [y].empty ()) {
							dirs.append (qMakePair (1, 0));
						}
						if (xi <= 0 && game.tiles [x - 1] [y].empty ()) {
							dirs.append (qMakePair (-1, 0));
						}
						if (yi >= 0 && game.tiles [x] [y + 1].empty ()) {
							dirs.append (qMakePair (0, 1));
						}
						if (yi <= 0 && game.tiles [x] [y - 1].empty ()) {
							dirs.append (qMakePair (0, -1));
						}
					}
					if (dirs.isEmpty ()) {
						if (game.tiles [x - this->xi] [y - this->yi].empty ()) {
							dirs.append (qMakePair (-this->xi, -this->yi));
						}
					}
					if (dirs.isEmpty ()) {
						this->xi = 0;
						this->yi = 0;
					} else {
						QPair <int, int> dir = dirs.at (rand () % dirs.count ());
						this->xi = dir.first;
						this->yi = dir.second;
					}
				}
				if (this->type == MonsterEnum::Lithor) {
					for (const Player &player : game.players) {
						if (player.x > this->x + 16 && player.x < this->x + 80
						 && player.y > this->y - 16 && player.y < this->y + 16) {
							this->shootingDir = 1;
							this->shootingTimer = 48;
						}
						if (player.x > this->x - 16 && player.x < this->x + 16
						 && player.y > this->y + 16 && player.y < this->y + 80) {
							this->shootingDir = 2;
							this->shootingTimer = 48;
						}
						if (player.x > this->x - 48 && player.x < this->x - 16
						 && player.y > this->y - 16 && player.y < this->y + 16) {
							this->shootingDir = 3;
							this->shootingTimer = 48;
						}
						if (player.x > this->x - 16 && player.x < this->x + 16
						 && player.y > this->y - 48 && player.y < this->y - 16) {
							this->shootingDir = 4;
							this->shootingTimer = 48;
						}
					}
				}
				this->x += xi;
				this->y += yi;
			} else {
				this->shootingTimer--;
				if (shootingTimer == 32) {
					Monster *monster;
					switch (this->shootingDir) {
					case 1:
						monster = new Monster (MonsterEnum::LithorFire, this->x + this->w, this->y, 64, 32);
						break;
					case 2:
						monster = new Monster (MonsterEnum::LithorFire, this->x, this->y + this->h, 32, 64);
						break;
					case 3:
						monster = new Monster (MonsterEnum::LithorFire, this->x - 64, this->y, 64, 32);
						break;
					default:
						monster = new Monster (MonsterEnum::LithorFire, this->x, this->y - 64, 32, 64);
						break;
					}
					monster->shootingDir = this->shootingDir;
					monster->shootingTimer = 32;
					game.monsters.append (monster);
				}
			}
			if (game.tiles [(this->x + 16) / 32] [(this->y + 16) / 32].fireCount) {
				this->alive = false;
			}
			break;
		case MonsterEnum::LithorFire:
			this->shootingTimer--;
			if (this->shootingTimer == 0) {
				this->alive = false;
			}
			break;
		default:
			break;
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
	if (tile.item == ItemEnum::PlusBomb) {
		this->bombCount++;
		tile.item = ItemEnum::None;
	} else if (tile.item == ItemEnum::PlusLength) {
		this->bombLength++;
		tile.item = ItemEnum::None;
	} else if (tile.item == ItemEnum::PlusLife) {
		this->lives++;
		tile.item = ItemEnum::None;
	}
	if (this->immuneCount) {
		this->immuneCount--;
	}
	for (Monster *monster : game->monsters) {
		if (this->x > monster->x - 16 &&
			this->x < monster->x + monster->w - 16 &&
			this->y > monster->y - 16 &&
			this->y < monster->y + monster->h - 16 &&
			(monster->type == MonsterEnum::Cr || monster->type == MonsterEnum::LithorFire)
		) {
			this->hit ();
		}
	}
	if (game->tiles [x] [y].fireCount) {
		this->hit ();
	}
}
void GameModel::Player::hit () {
	if (!this->immuneCount) {
		this->lives--;
		this->immuneCount = 31;
	}
}
const GameModel::Player *GameModel::Monster::closestPlayer (GameModel &model) const {
	QList <const Player *> players;
	int distance = 1000000;
	for (const Player &player : model.players) {
		int d = std::abs (player.x - this->x) + std::abs (player.y - this->y);
		if (d < distance) {
			players = {&player};
			distance = d;
		} else if (d == distance) {
			players.append (&player);
		}
	}
	if (players.isEmpty ()) {
		return nullptr;
	} else {
		return players.at (rand () % players.count ());
	}
}
