// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#include "Window.h"

#include <QMenuBar>

Window::Window (QWidget *parent) : QMainWindow (parent) {
	this->m_bar = new QMenuBar (this);
	QMenu *fileMenu = this->m_bar->addMenu ("&File");
	fileMenu->addAction (
		tr ("&New"),
		this, SLOT (newGame ()),
		tr ("Ctrl+N")
	);
	fileMenu->addAction (
		tr ("&Full screen"),
		this, SLOT (fullScreen ()),
		tr ("F11")
	);
	fileMenu->addAction (
		tr ("E&xit"),
		this, SLOT (close ()),
		tr ("Ctrl+Q")
	);
	this->setMenuBar (this->m_bar);
	this->newGame ();
}
void Window::fullScreen () {
	if (this->isFullScreen ()) {
		this->setMenuBar (m_bar);
		this->showNormal ();
	} else {
		//this->setMenuBar (0);
		this->showFullScreen ();
	}
}
void Window::newGame () {
	this->m_game = new GameView (this);
	this->setCentralWidget (this->m_game);
}
