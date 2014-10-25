// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QGLWidget>

#include "GameModel.h"

class GameView : public QGLWidget {
	Q_OBJECT
public:
	GameView (QWidget *parent = 0);
	~GameView () {
		qDebug ("XX");
	}

protected:
	void paintGL ();
	void resizeGL (int w, int h);
	void initializeGL ();
	void key (QKeyEvent *event, bool pressed);
	void keyPressEvent (QKeyEvent *event);
	void keyReleaseEvent (QKeyEvent *event);
	void timerEvent (QTimerEvent *);
private:

	void draw (GLuint tex, int x, int y, int width, int height);
	void load (int index, QString name);
	GLuint m_texs [32];
	
	GameModel model;
};
