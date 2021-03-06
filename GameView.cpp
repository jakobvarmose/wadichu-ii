// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#include "GameView.h"

#include <QKeyEvent>
#include "LevelSet.h"

GameView::GameView (QWidget *parent) : QGLWidget ([] {
	QGLFormat format;
	format.setSwapInterval (1);
	return format;
} (), parent) {
	LevelSet set3;
	set3.load ("../test.lvls");
	this->model.load (set3.levels.at (0));
	
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
			if (tile.type == TypeEnum::Solid) {
				this->draw (this->m_texs [1], 32 * x, 32 * y, 32, 32);
			} else if (tile.type == TypeEnum::Statue) {
				this->draw (this->m_texs [2], 32 * x, 32 * y, 32, 32);
			} else if (tile.type == TypeEnum::Immune) {
				this->draw (this->m_texs [3], 32 * x, 32 * y, 32, 32);
			} else if (tile.type == TypeEnum::Empty) {
				if (tile.item == ItemEnum::PlusBomb) {
					this->draw (this->m_texs [21], 32 * x, 32 * y, 32, 32);
				} else if (tile.item == ItemEnum::PlusLength) {
					this->draw (this->m_texs [22], 32 * x, 32 * y, 32, 32);
				} else if (tile.item == ItemEnum::PlusLife) {
					this->draw (this->m_texs [23], 32 * x, 32 * y, 32, 32);
				}
			}
			if (this->model.tiles [x] [y].fireCount) {
				this->draw (this->m_texs [7], 32 * x, 32 * y, 32, 32);
			}
		}
	}
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			const GameModel::Tile &tile = this->model.tiles [x] [y];
			if (tile.bomb) {
				this->draw (this->m_texs [5], 32 * x, 32 * y, 32, 32);
			}
		}
	}
	for (const GameModel::Monster *monster : this->model.monsters) {
		if (monster->type == MonsterEnum::Cr) {
			this->draw (this->m_texs [11], monster->x, monster->y, 32, 32);
		} else if (monster->type == MonsterEnum::Fish) {
			glBindTexture (GL_TEXTURE_2D, this->m_texs [12]);
			glPushMatrix ();
			glTranslatef (monster->x, monster->y, 0);
			glScalef (32, 32, 1);
			glBegin (GL_TRIANGLE_STRIP);
			if (monster->xi < 0) {
				glTexCoord2f (0, 0); glVertex2f (0, 0);
				glTexCoord2f (1, 0); glVertex2f (1, 0);
				glTexCoord2f (0, 1); glVertex2f (0, 1);
				glTexCoord2f (1, 1); glVertex2f (1, 1);
			} else if (monster->xi > 0) {
				glTexCoord2f (1, 0); glVertex2f (0, 0);
				glTexCoord2f (0, 0); glVertex2f (1, 0);
				glTexCoord2f (1, 1); glVertex2f (0, 1);
				glTexCoord2f (0, 1); glVertex2f (1, 1);
			} else if (monster->yi < 0) {
				glTexCoord2f (0, 1); glVertex2f (0, 0);
				glTexCoord2f (0, 0); glVertex2f (1, 0);
				glTexCoord2f (1, 1); glVertex2f (0, 1);
				glTexCoord2f (1, 0); glVertex2f (1, 1);
			} else {
				glTexCoord2f (1, 1); glVertex2f (0, 0);
				glTexCoord2f (1, 0); glVertex2f (1, 0);
				glTexCoord2f (0, 1); glVertex2f (0, 1);
				glTexCoord2f (0, 0); glVertex2f (1, 1);
			}
			glEnd ();
			glPopMatrix ();
		} else if (monster->type == MonsterEnum::Lithor) {
			this->draw (this->m_texs [13], monster->x, monster->y, 32, 32);
			/*if (monster->shootingDir != 0) {
				glBindTexture (GL_TEXTURE_2D, this->m_texs [14]);
				glPushMatrix ();
				glTranslatef (monster->x + 16, monster->y + 16, 0);
				glScalef (32, 32, 1);
				glRotatef ((monster->shootingDir - 1) * 90, 0, 0, 1);
				glBegin (GL_TRIANGLE_STRIP);
				glTexCoord2f (0, 0); glVertex2f (-0.5,  0.5);
				glTexCoord2f (1, 0); glVertex2f ( 0.5,  0.5);
				glTexCoord2f (0, 1); glVertex2f (-0.5,  2.5);
				glTexCoord2f (1, 1); glVertex2f ( 0.5,  2.5);
				glEnd ();
				glPopMatrix ();
			}*/
		} else if (monster->type == MonsterEnum::LithorFire) {
			this->draw (this->m_texs [14], monster->x, monster->y, monster->w, monster->h);
		}
	}
	for (const GameModel::Player &player : this->model.players) {
		if (player.immuneCount % 5 == 1) {
			this->draw (this->m_texs [94], player.x, player.y, 32, 32);
		} else {
			this->draw (this->m_texs [4], player.x, player.y, 32, 32);
		}
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
	this->load (2, "0012.png");
	this->load (3, "0021.png");
	this->load (4, "0016.png");
	this->load (5, "0034.bmp");
	this->load (7, "fire.png");
	this->load (11, "0027.bmp");
	this->load (12, "0028.bmp");
	this->load (13, "0038.bmp");
	this->load (14, "0030.bmp");
	this->load (21, "0022.bmp");
	this->load (22, "0031.bmp");
	this->load (23, "0015.png");
	this->load (94, "0016A.png");
	glClearColor (0, 0, 0, 1);
	glBlendFunc (GL_ONE, GL_ONE);
	glEnable (GL_BLEND);
	glDisable (GL_DEPTH_TEST);
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
		this->model.players [0].keyUp = pressed;
		break;
	case Qt::Key_A:
		this->model.players [0].keyLeft = pressed;
		break;
	case Qt::Key_S:
		this->model.players [0].keyDown = pressed;
		break;
	case Qt::Key_D:
		this->model.players [0].keyRight = pressed;
		break;
	case Qt::Key_Shift:
		this->model.players [0].keyBomb = pressed;
		break;
	case Qt::Key_Up:
		this->model.players [1].keyUp = pressed;
		break;
	case Qt::Key_Left:
		this->model.players [1].keyLeft = pressed;
		break;
	case Qt::Key_Down:
		this->model.players [1].keyDown = pressed;
		break;
	case Qt::Key_Right:
		this->model.players [1].keyRight = pressed;
		break;
	case Qt::Key_Space:
		this->model.players [1].keyBomb = pressed;
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
