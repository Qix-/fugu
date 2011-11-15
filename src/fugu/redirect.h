#ifndef FGE_REDIRECT_H
#define FGE_REDIRECT_H

#include <QThread>
#include <QString>

#ifndef _WIN32
//
// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License:  Public domain
//

#include <QObject>

class QTextIStream;
class QSocketNotifier;

class Interceptor : public QObject
{
    Q_OBJECT
    public:
    Interceptor(QObject * p = 0);
    ~Interceptor();
    public slots:
    void initialize(int outFd = 1); // defaults to standard output
    void received();
    void finish();
    QTextIStream *textIStream() {return m_stream;};
    signals:
    void received(QTextIStream *);
    private:
    QTextIStream * m_stream;
    QSocketNotifier * m_notifier;
    int m_pipeFd[2];
    int m_origFd;
    int m_origFdCopy;
};

#endif


class StdOutRedirector : public QThread
{
	Q_OBJECT

    public:
	StdOutRedirector(QObject* parent):QThread(parent){}
    	void run();
    signals:
    	void caughtString(QString);
#ifndef _WIN32
    public slots:
	void displayString();

public:
	Interceptor* stdoutInterceptor;
	Interceptor* stderrInterceptor;
#endif

	
};

StdOutRedirector* redirectConsoleOutput(QObject* p);

#endif
