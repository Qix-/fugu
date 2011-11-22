#ifndef FUGU_QREDIRECTORIMPL_POSIX_H
#define FUGU_QREDIRECTORIMPL_POSIX_H

#ifndef _WIN32

#include <QThread>
#include <QString>
#include <QObject>
#include <QTextIStream>
#include <QSocketNotifier>

#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <cassert>

using namespace std; // safe. as we are just #including in one .cpp file

// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License:  Public domain

class Interceptor : public QObject
{
	Q_OBJECT
public:
	IInterceptor(QObject * parent = 0) :
		QObject(parent),
		m_stream(0),
		m_notifier(0)
	{
	}
	~Interceptor()
	{
		finish();
	}
public slots:
void initialize(int outFd=1)
{
	if (m_notifier != 0) finish();

	m_origFd = outFd;

	// m_pipefd[0] is the read-end of the pipe and m_pipefd[1] is the write-end.

	// Open a new pipe. We will pipe output file descriptor through the new pipe and
	// read it with a QTextIStream object, using a QSocketNotifier to tell us when
	// something is waiting in the pipe.
	int rc = ::pipe(m_pipeFd);
	assert (rc >= 0);

	// Save the original output descriptor.
	m_origFdCopy = ::dup(m_origFd);
	assert(m_origFdCopy >= 0);

	// Make the ouput descriptor a copy of pipe's write end
	rc = ::dup2(m_pipeFd[1], m_origFd);
	assert (rc >= 0);
	::close(m_pipeFd[1]); // close the write end of the pipe descriptor, it's redundant now

	// Open the pipe's read end non-blocking so that we can reliably get the data
	rc = ::fcntl(m_pipeFd[0], F_GETFL);
	assert(rc != -1);
	rc = ::fcntl(m_pipeFd[0], F_SETFL, rc | O_NONBLOCK); // otherwise atEnd() will block!
	assert(rc != -1);
	FILE * f = fdopen(m_pipeFd[0], "r");
	assert(f != 0);

	if (m_stream != 0) delete m_stream;
	if (m_notifier != 0) delete m_notifier;
	m_stream = new QTextIStream(f);
	m_notifier = new QSocketNotifier(m_pipeFd[0], QSocketNotifier::Read);
	QObject::connect(m_notifier, SIGNAL(activated(int)), SLOT(received()));
}
void received()
{
	emit received(m_stream);
}
void finish()
{
	if (m_notifier == 0) return;

	std::cout.flush(); // flush standard output cout file descriptor
	::fflush(NULL);    // flush all file buffers
	::fsync(1);        // syncronize standard output buffers -- may be unnessessery
	received();        // process whatever data is left there

	// Restore original state
	delete m_notifier;
	delete m_stream;
	m_notifier = 0;
	m_stream = 0;

	::dup2(m_origFdCopy, m_origFd); // restore the output descriptor
	::close(m_origFdCopy); // close the copy as it's redundant now
	::close(m_pipeFd[0]);  // close the reading end of the pipe
}
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

class QRedirectorImpl {
	Q_OBJECT

public:

	QRedirectorImpl(){
		stdoutInterceptor = new Interceptor();
		stdoutInterceptor->initialize(1);
		stderrInterceptor = new Interceptor();
		stderrInterceptor->initialize(2);

		connect(stdoutInterceptor, SIGNAL(received(QTextIStream *)), this, SLOT(displayString()));
		connect(stderrInterceptor, SIGNAL(received(QTextIStream *)), this, SLOT(displayString()));
	}

	~QRedirectorImpl(){
		stdoutInterceptor->finish();
		stderrInterceptor->finish();

		delete stdoutInterceptor;
		delete stderrInterceptor;
	}

public slots:
	void displayString(){
		//flush the stdout/stderr before displaying the prompt
		if (stdoutInterceptor)
		{
			QTextIStream* s = stdoutInterceptor->textIStream();

			QString line;
	#if QT_VERSION >= 0x040000
			line = s->readAll();
			emit (caughtString(line));
	#else
			while (!s->atEnd())
			{
				line = s->readLine();
				emit (out(line));
			}
	#endif
		}

		if (stderrInterceptor)
		{
			QTextIStream* s = stderrInterceptor->textIStream();

			QString line;
	#if QT_VERSION >= 0x040000
			line = s->readAll();
			emit (caughtString(line));
	#else
			while (!s->atEnd())
			{
				line = s->readLine();
				emit (err(line));
			}
	#endif
		}

	}

signals:
	void out(QString str);
	void err(QString str);

public:
	Interceptor* stdoutInterceptor;
	Interceptor* stderrInterceptor;
};

#endif // NOT WIN32

#endif
