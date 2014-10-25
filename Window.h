// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QMainWindow>
#include "GameView.h"

class Window : public QMainWindow {
	Q_OBJECT
public:
	Window (QWidget *parent = 0);
private slots:
	void fullScreen ();
	void newGame ();
private:
	GameView *m_game;
	QMenuBar *m_bar;
};
