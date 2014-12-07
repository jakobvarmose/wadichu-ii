// Copyright 2014 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QtGlobal>

enum class TypeEnum : quint8  {
	Empty,
	Solid,
	Statue,
	Immune
};

enum class ItemEnum : quint8  {
	None,
	PlusBomb,
	PlusLength,
	PlusLife
};

enum class MonsterEnum : quint8 {
	Cr = 1,
	Fish,
	Lithor,
	LithorFire,
	Fireball
};
