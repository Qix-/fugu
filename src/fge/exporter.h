#ifndef FGE_EXPORTER_H
#define FGE_EXPORTER_H

#include "fg/universe.h"
#include <QFile>
#include <QDir>
#include <QString>

class Exporter {
public:
	// factories
	static Exporter ExportFrameToObj(fg::Universe*);

	// properties
	Exporter& dir(QDir directory){mDirectory = directory; return *this;}

	/// \brief run the exporter, returns false if an error occurs
	bool run();

	/// \brief return a string describing the error that occurred when running..
	QString error() const {return QString("Unknown error");}
protected:
	Exporter(fg::Universe*);
	void exportFrameToObj(QFile);

	fg::Universe* mUniverse;
	enum Type {OBJ} mType;
	QDir mDirectory;
};



#endif
