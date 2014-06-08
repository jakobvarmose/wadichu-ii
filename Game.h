// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#pragma once

#include <QGLWidget>

class Game : public QGLWidget {
	Q_OBJECT
public:
	Game (QWidget *parent = 0);
	~Game () {
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
	class Bomb {
	public:
		int m_x, m_y;
		int m_length;
		int m_count;
		Bomb () : m_x (0), m_y (0), m_count (0), m_length (2) {
		}
		Bomb (
			int x,
			int y,
			int count
		) :
			m_x (x),
			m_y (y),
			m_count (count),
			m_length (2)
		{
		}
	};
	class Player {
	public:
		int m_x, m_y;
		bool m_keyUp, m_keyLeft, m_keyDown, m_keyRight;
		bool m_keyBomb;
		int m_bombCount;
		Player () :
			m_x (0), m_y (0),
			m_keyUp (false), m_keyLeft (false),
			m_keyDown (false), m_keyRight (false),
			m_keyBomb (false), m_bombCount (10)
		{
		}
		void update (Game *game);
	};
	class Fire {
	public:
		int m_x, m_y;
	};
	class Square {
	public:
		int m_type;
		bool m_fire;
		bool m_bomb;
		Square () : m_type (3), m_fire (false), m_bomb (false) {
		}
		bool empty () {
			return this->m_type == 0 && this->m_bomb == false;
		}
		bool solid () {
			return this->m_type == 3;
		}
	};
	class Monster {
	public:
		int m_x, m_y;
		Monster (int x, int y) : m_x (x), m_y (y) {
		}
	};

	void fire (QSet<int> &res, int x, int y, int xi, int yi, int length);
	void draw (GLuint tex, int x, int y, int width, int height);
	void load (int index, QString name);
	GLuint m_texs [32];
	
	Square map [19] [19];
	QVector <Bomb> m_bombs;
	QVector <Player> m_players;
	QVector <Fire *> m_fires;
	QVector <Monster *> m_monsters;
};
