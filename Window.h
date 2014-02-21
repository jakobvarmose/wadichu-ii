// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QMainWindow>
#include "Game.h"

class Window : public QMainWindow {
	Q_OBJECT
public:
	Window (QWidget *parent = 0);
private slots:
	void fullScreen ();
	void newGame ();
private:
	Game *m_game;
	QMenuBar *m_bar;
};
