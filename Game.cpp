// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#include "Game.h"

#include <QKeyEvent>

Game::Game (QWidget *parent) : QGLWidget (parent) {
	this->setMinimumSize (608, 608);
	this->m_players.resize (2);
	this->m_players [0].m_x = 1 * 32;
	this->m_players [0].m_y = 17 * 32;
	this->m_players [1].m_x = 17 * 32;
	this->m_players [1].m_y = 1 * 32;
	this->startTimer (20);
	this->grabKeyboard ();
	this->setFocus ();
}
void Game::draw (GLuint tex, int x, int y, int width, int height) {
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
void Game::paintGL () {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_TEXTURE_2D);
	for (int x = 0; x < 19; ++x) {
		for (int y = 0; y < 19; ++y) {
			int cell = this->map [x] [y].m_type;
			if (cell == 3) {
				this->draw (this->m_texs [3], 32 * x, 32 * y, 32, 32);
			} else if (cell == 1) {
				this->draw (this->m_texs [1], 32 * x, 32 * y, 32, 32);
			}
			if (this->map [x] [y].m_fire) {
				this->draw (this->m_texs [7], 32 * x, 32 * y, 32, 32);
			}
		}
	}
	foreach (Bomb bomb, this->m_bombs) {
		this->draw (this->m_texs [5], bomb.m_x, bomb.m_y, 32, 32);
	}
	foreach (Player player, this->m_players) {
		this->draw (this->m_texs [4], player.m_x, player.m_y, 32, 32);
	}
	this->update ();
}
void Game::resizeGL (int width, int height) {
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
void Game::initializeGL () {
	this->load (1, "0029.png");
	this->load (3, "0021.png");
	this->load (4, "0016.png");
	this->load (5, "0034.bmp");
	this->load (7, "fire.png");
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
				map [x] [y].m_type = 3;
			} else if (
				(x <= 2 || x >= 16) &&
				(y <= 2 || y >= 16)
			) {
				map [x] [y].m_type = 0;
			} else {
				if (rand () % 1000 < 700) {
					map [x] [y].m_type = 0;
				} else {
					map [x] [y].m_type = 1;
				}
			}
		}
	}
}
void Game::load (int index, QString name) {
	m_texs [index] = this->bindTexture (
		QString ("../wadichu-ii/" + name),
		GL_TEXTURE_2D
	);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
void Game::key (QKeyEvent *event, bool pressed) {
	event->accept ();
	switch (event->key ()) {
	case Qt::Key_W:
		this->m_players [0].m_keyUp = pressed;
		break;
	case Qt::Key_A:
		this->m_players [0].m_keyLeft = pressed;
		break;
	case Qt::Key_S:
		this->m_players [0].m_keyDown = pressed;
		break;
	case Qt::Key_D:
		this->m_players [0].m_keyRight = pressed;
		break;
	case Qt::Key_Shift:
		this->m_players [0].m_keyBomb = pressed;
		break;
	case Qt::Key_Up:
		this->m_players [1].m_keyUp = pressed;
		break;
	case Qt::Key_Left:
		this->m_players [1].m_keyLeft = pressed;
		break;
	case Qt::Key_Down:
		this->m_players [1].m_keyDown = pressed;
		break;
	case Qt::Key_Right:
		this->m_players [1].m_keyRight = pressed;
		break;
	case Qt::Key_Space:
		this->m_players [1].m_keyBomb = pressed;
		break;
	}
}
void Game::keyPressEvent (QKeyEvent *event) {
	key (event, true);
}
void Game::keyReleaseEvent (QKeyEvent *event) {
	key (event, false);
}
void Game::Player::update (Game *game) {
	int speed = 4;
	int dx = this->m_keyRight - this->m_keyLeft;
	int dy = this->m_keyUp - this->m_keyDown;
	if (dx != 0) {
		if (this->m_x % 32 == 0) {
			int x = this->m_x / 32 + dx;
			bool a = game->map [x] [this->m_y / 32].empty ();
			bool b = game->map [x] [(this->m_y + 31) / 32].empty ();
			if (a && b) {
				this->m_x += dx * speed;
			} else if (a) {
				this->m_y += -1 * speed;
			} else if (b) {
				this->m_y += 1 * speed;
			}
		} else {
			this->m_x += dx * speed;
		}
	}
	if (dy != 0) {
		if (this->m_y % 32 == 0) {
			int y = this->m_y / 32 + dy;
			bool a = game->map [this->m_x / 32] [y].empty ();
			bool b = game->map [(this->m_x + 31) / 32] [y].empty ();
			if (a && b) {
				this->m_y += dy * speed;
			} else if (a) {
				this->m_x += -1 * speed;
			} else if (b) {
				this->m_x += 1 * speed;
			}
		} else {
			this->m_y += dy * speed;
		}
	}
	int x = (this->m_x + 16) / 32;
	int y = (this->m_y + 16) / 32;
	if (this->m_keyBomb && this->m_bombCount > 0 && game->map [x] [y].empty ()) {
		this->m_bombCount -= 1;
		game->map [x] [y].m_bomb = true;
		game->m_bombs.append (Game::Bomb (
			(this->m_x + 16) / 32 * 32,
			(this->m_y + 16) / 32 * 32,
			100
		));
	}
}
void Game::fire (QSet<int> &res, int x, int y, int xi, int yi, int length) {
	for (int i = 0; i < length && this->map [x] [y].empty (); ++i) {
		x += xi;
		y += yi;
		if (!this->map [x] [y].solid ()) {
			res.insert (100 * x + y);
		}
	}
}

void Game::timerEvent (QTimerEvent *) {
	for (int i = 0; i != this->m_bombs.size (); ++i) {
		this->m_bombs [i].m_count -= 1;
		if (this->m_bombs [i].m_count == 0) {
			this->map [
				this->m_bombs [i].m_x / 32
			] [
				this->m_bombs [i].m_y / 32
			].m_bomb = false;
			QSet<int> res;
			res.insert (
				100 * (this->m_bombs [i].m_x / 32) +
				(this->m_bombs [i].m_y / 32)
			);
			fire (
				res,
				this->m_bombs [i].m_x / 32,
				this->m_bombs [i].m_y / 32,
				1, 0,
				this->m_bombs [i].m_length
			);
			fire (
				res,
				this->m_bombs [i].m_x / 32,
				this->m_bombs [i].m_y / 32,
				-1, 0,
				this->m_bombs [i].m_length
			);
			fire (
				res,
				this->m_bombs [i].m_x / 32,
				this->m_bombs [i].m_y / 32,
				0, 1,
				this->m_bombs [i].m_length
			);
			fire (
				res,
				this->m_bombs [i].m_x / 32,
				this->m_bombs [i].m_y / 32,
				0, -1,
				this->m_bombs [i].m_length
			);
			foreach (int i, res) {
				int x = i / 100;
				int y = i % 100;
				this->map [x] [y].m_fire = true;
			}
			this->m_bombs.remove (i);
			--i;
		}
	}
	for (int i = 0; i != this->m_players.size (); ++i) {
		this->m_players [i].update (this);
	}
}
