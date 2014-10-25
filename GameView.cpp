// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#include "GameView.h"

#include <QKeyEvent>

GameView::GameView (QWidget *parent) : QGLWidget (parent) {
	this->setMinimumSize (608, 608);
	this->startTimer (20);
	this->grabKeyboard ();
	this->setFocus ();
}
void GameView::draw (GLuint tex, int x, int y, int width, int height) {
	glBindTexture (GL_TEXTURE_2D, tex);
	glPushMatrix ();
	glTranslatef (x, y, 0);
	glScalef (width, height, 1);
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (0, 0); glVertex2f (0, 0);
	glTexCoord2f (1, 0); glVertex2f (1, 0);
	glTexCoord2f (0, 1); glVertex2f (0, 1);
	glTexCoord2f (1, 1); glVertex2f (1, 1);
	glEnd ();
	glPopMatrix ();
}
void GameView::paintGL () {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_TEXTURE_2D);
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			const GameModel::Tile &tile = this->model.tiles [x] [y];
			if (tile.type == 3) {
				this->draw (this->m_texs [3], 32 * x, 32 * y, 32, 32);
			} else if (tile.type == 1) {
				this->draw (this->m_texs [1], 32 * x, 32 * y, 32, 32);
			}
			if (this->model.tiles [x] [y].fire) {
				this->draw (this->m_texs [7], 32 * x, 32 * y, 32, 32);
			}
		}
	}
	for (const GameModel::Bomb &bomb : this->model.bombs) {
		this->draw (this->m_texs [5], bomb.x, bomb.y, 32, 32);
	}
	for (const GameModel::Monster *monster : this->model.monsters) {
		this->draw (this->m_texs [11], monster->x, monster->y, 32, 32);
	}
	for (const GameModel::Player &player : this->model.players) {
		this->draw (this->m_texs [4], player.x, player.y, 32, 32);
	}
	this->update ();
}
void GameView::resizeGL (int width, int height) {
	int size = qMin (width, height);
	int left = (width - size) / 2;
	int top = (height - size) / 2;
	//glViewport (0, 0, width, height);
	glViewport (left, top, size, size);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0, 608, 0, 608, -100, 100);
	//glOrtho (0, width, 0, height, -100, 100);
	glMatrixMode (GL_MODELVIEW);
}
void GameView::initializeGL () {
	this->load (1, "0029.png");
	this->load (3, "0021.png");
	this->load (4, "0016.png");
	this->load (5, "0034.bmp");
	this->load (7, "fire.png");
	this->load (11, "0027.bmp");
	glClearColor (0, 0, 0, 1);
	glBlendFunc (GL_ONE, GL_ONE);
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			if (
				x == 0 ||
				x == 18 ||
				y == 0 ||
				y == 18 ||
				x % 2 == 0 && y % 2 == 0
			) {
				model.tiles [x] [y].type = 3;
			} else if (
				(x <= 2 || x >= 16) &&
				(y <= 2 || y >= 16)
			) {
				model.tiles [x] [y].type = 0;
			} else {
				if (rand () % 1000 < 700) {
					model.tiles [x] [y].type = 0;
				} else {
					model.tiles [x] [y].type = 1;
				}
			}
		}
	}
}
void GameView::load (int index, QString name) {
	m_texs [index] = this->bindTexture (
		QString ("../wadichu-ii/" + name),
		GL_TEXTURE_2D
	);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
void GameView::key (QKeyEvent *event, bool pressed) {
	event->accept ();
	switch (event->key ()) {
	case Qt::Key_W:
		this->model.players [0].m_keyUp = pressed;
		break;
	case Qt::Key_A:
		this->model.players [0].m_keyLeft = pressed;
		break;
	case Qt::Key_S:
		this->model.players [0].m_keyDown = pressed;
		break;
	case Qt::Key_D:
		this->model.players [0].m_keyRight = pressed;
		break;
	case Qt::Key_Shift:
		this->model.players [0].m_keyBomb = pressed;
		break;
	case Qt::Key_Up:
		this->model.players [1].m_keyUp = pressed;
		break;
	case Qt::Key_Left:
		this->model.players [1].m_keyLeft = pressed;
		break;
	case Qt::Key_Down:
		this->model.players [1].m_keyDown = pressed;
		break;
	case Qt::Key_Right:
		this->model.players [1].m_keyRight = pressed;
		break;
	case Qt::Key_Space:
		this->model.players [1].m_keyBomb = pressed;
		break;
	}
}
void GameView::keyPressEvent (QKeyEvent *event) {
	key (event, true);
}
void GameView::keyReleaseEvent (QKeyEvent *event) {
	key (event, false);
}
void GameView::timerEvent (QTimerEvent *) {
	this->model.update ();
}
