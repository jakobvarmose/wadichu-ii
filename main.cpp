// Copyright 2013 Jakob Varmose Bentzen | Released under the MIT License
#include "main.h"

#include <QApplication>
#include "Window.h"

#include <QtDebug>
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
int main (int argc, char **argv) {
	QApplication app (argc, argv);
	app.setApplicationName ("Wadichu II");
	Phonon::MediaObject media;
	Phonon::AudioOutput output (Phonon::MusicCategory);
	Phonon::createPath (&media, &output);
	//media.enqueue (Phonon::MediaSource ("../tracks/DST-Aurora.mp3"));
	media.play ();
	Window win;
	win.show ();
	//win.showFullScreen ();
	return app.exec ();
}
