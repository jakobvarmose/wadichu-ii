// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#include "GameModel.h"

#include <QDataStream>
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
				this->tiles [x] [y].type = TypeEnum::Immune;
				this->tiles [x] [y].item = ItemEnum::None;
			} else if (
				(x <= 2 || x >= 16) &&
				(y <= 2 || y >= 16)
			) {
				this->tiles [x] [y].type = TypeEnum::Empty;
				this->tiles [x] [y].item = ItemEnum::None;
			} else {
				int r = rand () % 1000;
				if (r < 700) {
					this->tiles [x] [y].type = TypeEnum::Empty;
					this->tiles [x] [y].item = ItemEnum::None;
				} else if (r < 720) {
					this->tiles [x] [y].type = TypeEnum::Statue;
					this->tiles [x] [y].item = ItemEnum::None;
				} else {
					this->tiles [x] [y].type = TypeEnum::Solid;
					int q = rand () % 1000;
					if (q < 200) {
						this->tiles [x] [y].item = ItemEnum::PlusBomb;
					} else if (q < 400) {
						this->tiles [x] [y].item = ItemEnum::PlusLength;
					} else if (q < 420) {
						this->tiles [x] [y].item = ItemEnum::PlusLife;
					} else {
						this->tiles [x] [y].item = ItemEnum::None;
					}
				}
			}
		}
	}
	for (int i = 0; i < 10; ++i) {
		MonsterEnum type;
		int q = rand () % 1000;
		if (q < 250) {
			type = MonsterEnum::Cr;
		} else if (q < 500) {
			type = MonsterEnum::Fish;
		} else {
			type = MonsterEnum::Lithor;
		}
		this->monsters.push_back (new Monster (type, 64 * i + 32, 32));
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
						this->tiles [x] [y].fire = true;
						this->tiles [x] [y].fireCount = 25;
					}
					tile.bomb = false;
				}
			}
		}
	}
	for (int i = 0; i < this->monsters.size (); ++i) {
		this->monsters [i]->update (*this);
		if (!this->monsters [i]->alive && !this->tiles [(this->monsters [i]->x + 16) / 32] [(this->monsters [i]->y + 16) / 32].fire) {
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
		if (this->x % 32 == 0 && this->y % 32 == 0) {
			int x = this->x / 32;
			int y = this->y / 32;
			QList <QPair <int, int>> dirs;
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
			if (this->type == MonsterEnum::Lithor) {
				for (const Player &player : game.players) {
					if (player.x > this->x + 16 && player.x < this->x + 80
					 && player.y > this->y - 16 && player.y < this->y + 16) {
						this->shootingTimer = 100;
						this->shootingDir = 4;
						this->xi = 0;
						this->yi = 0;
					}
				}
			}
		}
		this->x += xi;
		this->y += yi;
		if (game.tiles [(this->x + 16) / 32] [(this->y + 16) / 32].fire) {
			this->alive = false;
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
}
LevelSet::LevelSet (const QByteArray &data) {
	QDataStream stream (data);
	stream >> this->version;
	quint8 levelsSize;
	stream >> levelsSize;
	for (int i = 0; i < levelsSize; ++i) {
		Level level;
		for (int x = 0; x < 19; ++x) {
			for (int y = 0; y < 19; ++y) {
				stream >> level.tiles [x] [y].type;
				stream >> level.tiles [x] [y].item;
			}
		}
		quint8 monstersSize;
		stream >> monstersSize;
		for (int i = 0; i < monstersSize; ++i) {
			Level::Monster monster;
			stream >> monster.type;
			stream >> monster.x;
			stream >> monster.y;
			level.monsters.append (monster);
		}
		this->levels.append (level);
	}
}
QByteArray LevelSet::serialize () const {
	QByteArray data;
	QDataStream stream (&data, QIODevice::WriteOnly);
	stream << version;
	stream << quint8 (levels.size ());
	for (const Level &level : this->levels) {
		for (int x = 0; x < 19; ++x) {
			for (int y = 0; y < 19; ++y) {
				const Level::Tile &tile = level.tiles [x] [y];
				stream << tile.type;
				stream << tile.item;
			}
		}
		stream << quint8 (level.monsters.size ());
		for (const Level::Monster &monster : level.monsters) {
			stream << monster.type;
			stream << monster.x;
			stream << monster.y;
		}
	}
	return data;
}
