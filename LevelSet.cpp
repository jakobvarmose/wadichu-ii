// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#include "LevelSet.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

void Level::random () {
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
		this->monsters.append ({type, 9, 9});
	}
}
LevelSet::LevelSet () {
}
QJsonObject Level::toJson () const {
	QJsonObject result;
	QJsonArray tiles;
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			const Tile &tile = this->tiles [x] [y];
			QJsonObject buffer;
			buffer.insert ("type", int (tile.type));
			buffer.insert ("item", int (tile.item));
			tiles.append (buffer);
		}
	}
	result.insert ("tiles", tiles);
	QJsonArray monsters;
	for (const Monster &monster : this->monsters) {
		QJsonObject buffer;
		buffer.insert ("type", int (monster.type));
		buffer.insert ("x", monster.x);
		buffer.insert ("y", monster.y);
		monsters.append (buffer);
	}
	result.insert ("monsters", monsters);
	return result;
}
Level Level::fromJson (const QJsonObject &data) {
	Level level;
	QJsonArray tiles = data.value ("tiles").toArray ();
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			int i = x * 19 + y;
			if (i < tiles.count ()) {
				QJsonObject tile = tiles.at (i).toObject ();
				level.tiles [x] [y].type = TypeEnum (tile.value ("type").toInt ());
				level.tiles [x] [y].type = TypeEnum::Empty;
				if (x == 0 || x == 18 || y == 0 || y == 18) {
					level.tiles [x] [y].type = TypeEnum::Immune;
				}
				level.tiles [x] [y].item = ItemEnum (tile.value ("item").toInt ());
			}
		}
	}
	QJsonArray monsters = data.value ("monsters").toArray ();
	for (int i = 0; i < monsters.count (); ++i) {
		QJsonObject monster = monsters.at (i).toObject ();
		MonsterEnum type = MonsterEnum (monster.value ("type").toInt ());
		int x = monster.value ("x").toInt ();
		int y = monster.value ("y").toInt ();
		level.monsters.append ({type, quint8 (x), quint8 (y)});
	}
	return level;
}
QJsonObject LevelSet::toJson () const {
	QJsonObject result;
	QJsonArray levels;
	for (const Level &level : this->levels) {
		levels.append (level.toJson ());
	}
	result.insert ("levels", levels);
	return result;
}
void LevelSet::load (const QString &filename) {
	QFile f (filename);
	f.open (QFile::ReadOnly);
	this->load (QJsonDocument::fromJson (qUncompress (f.readAll ())).object ());
}
void LevelSet::load (const QJsonObject &data) {
	QJsonArray levels = data.value ("levels").toArray ();
	for (const QJsonValue &level : levels) {
		this->levels.append (Level::fromJson (level.toObject ()));
	}
}
void LevelSet::save (const QString &filename) const {
	QFile f (filename);
	f.open (QFile::WriteOnly);
	f.write (qCompress (QJsonDocument (this->toJson ()).toJson (QJsonDocument::Compact), 9));
	f.close ();
}
