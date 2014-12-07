// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QList>
#include "Enum.h"
class QJsonObject;

struct Level {
	struct Tile {
		TypeEnum type;
		ItemEnum item;
	};
	struct Monster {
		MonsterEnum type;
		quint8 x;
		quint8 y;
	};
	Tile tiles [19] [19];
	QList <Monster> monsters;
	void random ();
	QJsonObject toJson () const;
	static Level fromJson (const QJsonObject &);
};

class LevelSet {
public:
	QList <Level> levels;
	quint16 version;
	LevelSet ();
	LevelSet (const QByteArray &data);
	void save (const QString &filename) const;
	void load (const QString &filename);
	void load (const QJsonObject &data);
	QJsonObject toJson () const;
	QByteArray serialize () const;
};
