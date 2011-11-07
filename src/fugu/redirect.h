#ifndef FGE_REDIRECT_H
#define FGE_REDIRECT_H

#include <QThread>
#include <QString>

class StdOutRedirector : public QThread
{
	Q_OBJECT

	public:
		StdOutRedirector(QObject* parent):QThread(parent){}
    	void run();
    signals:
    	void caughtString(QString);
};

StdOutRedirector* redirectConsoleOutput(QObject* p);

#endif
