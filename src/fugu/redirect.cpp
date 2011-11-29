#include "redirect.h"

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>

#include <QThread>
#include <QTextIStream>
#include <QSocketNotifier>

// we don't use the interceptor for win32
Interceptor::Interceptor(QObject * p):QObject(p){}
Interceptor::~Interceptor(){}
void Interceptor::initialize(int outFd){}
void Interceptor::received(){}
void Interceptor::finish(){}

// code from: http://www.gamedev.net/topic/532101-redirecting-console-output/
void RedirectConsole(HANDLE hPipe){
	int hConHandle = 0;
	HANDLE lStdHandle = 0;
	FILE *fp = 0 ;

	// redirect unbuffered STDOUT to the Pipe
	lStdHandle = hPipe;
	// hConHandle = _open_osfhandle(PtrToUlong(lStdHandle), _O_TEXT);
	hConHandle = _open_osfhandle(reinterpret_cast<intptr_t>(lStdHandle), _O_TEXT);
	fp = _fdopen(hConHandle, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}


 void StdOutRedirector::run()
 {
	HANDLE rd,wr;
	if (!CreatePipe(&rd,&wr,NULL,0))
		std::cerr << "Error creating std out redirection pipe.\n";
	RedirectConsole(wr);
	//std::cout << "cout\n";
	//printf("printf\n");
	char buf[100];
	while (true) {
		DWORD readBytes=0;
		if (!ReadFile(rd, buf, 99, &readBytes, 0))
			break;
		buf[99] = 0;

		if (readBytes>0){
			std::string str;
			for (DWORD i=0;i<readBytes;i++)
				str+=buf[i];
			emit (caughtString(QString::fromStdString(str)));
		}
		else
			break;
	}
 }

 void StdOutRedirector::flush(){
	 /*
	 char buf[100];
	 DWORD readBytes=0;
		if (!ReadFile(rd, buf, 99, &readBytes, 0))
			break;
		buf[99] = 0;

		if (readBytes>0){
			std::string str;
			for (DWORD i=0;i<readBytes;i++)
				str+=buf[i];
			emit (caughtString(QString::fromStdString(str)));
		}
		else
			break;*/
 }

 void StdOutRedirector::displayString(){}


 StdOutRedirector* redirectConsoleOutput(QObject* p){
	StdOutRedirector* th = new StdOutRedirector(p);
	return th;
}

#else // end _WIN32

#include <stdexcept>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <cassert>
using namespace std;



#include <QTextIStream>
#include <QSocketNotifier>



StdOutRedirector* redirectConsoleOutput(QObject * p){
	StdOutRedirector* th = new StdOutRedirector(p);
	return th;
}


void StdOutRedirector::run(){
	
	//Initialize the interceptors
	/* this code adapted from qconsole (wconsole.cpp) (c) Houssem BDIOUI under the GPL */
	stdoutInterceptor = new Interceptor();
	stdoutInterceptor->initialize(1);
	connect(stdoutInterceptor, SIGNAL(received(QTextIStream *)), this, SLOT(displayString()));

	stderrInterceptor = new Interceptor();
	stderrInterceptor->initialize(2);
	connect(stderrInterceptor, SIGNAL(received(QTextIStream *)), this, SLOT(displayString()));

	// stdoutInterceptor->moveToThread(this);
	// stderrInterceptor->moveToThread(this);

	// don't exit, just go into a loop ...
	while(true){
		msleep(100);
	}

	stdoutInterceptor->finish();
	stderrInterceptor->finish();
}
void StdOutRedirector::flush(){

}
void StdOutRedirector::displayString(){
/* this code adapted from qconsole (wconsole.cpp) (c) Houssem BDIOUI under the GPL */

	assert(false);

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
		emit (caughtString(line));
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
		emit (caughtString(line));
	    }
	#endif
    }


}

// Interceptor

//
// Author: Kuba Ober <kuba@mareimbrium.org>
// Downloaded from: http://www.ibib.waw.pl/~winnie
//
// License:  Public domain
//

Interceptor::Interceptor(QObject * parent) :
QObject(parent),
    m_stream(0),
    m_notifier(0)
{
}

Interceptor::~Interceptor()
{
    finish();
}

void Interceptor::initialize(int outFd)
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

void Interceptor::received()
{
    emit received(m_stream);
}

void Interceptor::finish()
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


#endif
