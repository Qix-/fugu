#ifndef FUGU_REDIRECT_H
#define FUGU_REDIRECT_H




// old

#include <QThread>
#include <QString>
#include <QObject>

#include <QTextIStream>
#include <QSocketNotifier>

//
// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License:  Public domain
//


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


class StdOutRedirector : public QThread
{
	Q_OBJECT

    public:
	StdOutRedirector(QObject* parent):QThread(parent){}
	void run();

    signals:
    void caughtString(QString);

	public slots:
	void flush();
	void displayString();

public:
	Interceptor* stdoutInterceptor;
	Interceptor* stderrInterceptor;
};

StdOutRedirector* redirectConsoleOutput(QObject* p);

#endif
