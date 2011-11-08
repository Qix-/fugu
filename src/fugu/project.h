#ifndef FUGU_PROJECT_H
#define FUGU_PROJECT_H

#include <QDir>
#include <QString>

struct Project {
	QString name;

	bool isTemp;
	QStringList files;

	QDir dir; // dir (relative to projects directory)

	QDir tempDir; // dir if temporary

	// other dynamic info
	// sliders ..
	// window settings ..
	// assets (textures, .objs, etc..)
};

#endif
