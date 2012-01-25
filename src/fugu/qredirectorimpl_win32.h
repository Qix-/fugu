#ifndef FUGU_QREDIRECTORIMPL_WIN32_H
#define FUGU_QREDIRECTORIMPL_WIN32_H

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>

class QRedirectorImpl {
public:
	HANDLE rd,wr;
	char buf[100];

	QRedirectorImpl(){
		if (!CreatePipe(&rd,&wr,NULL,0))
			std::cerr << "Error creating std out redirection pipe.\n";
		redirectConsole(wr);
	}

	// code from: http://www.gamedev.net/topic/532101-redirecting-console-output/
	inline void redirectConsole(HANDLE hPipe){
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

	// wait, and put the next captured std::out string in s, return false if finished
	inline bool nextOut(std::string& s){
		DWORD readBytes=0;
		if (!ReadFile(rd, buf, 99, &readBytes, 0))
			return false;
		buf[99] = 0;

		if (readBytes>0){
			s = std::string();
			for (DWORD i=0;i<readBytes;i++)
				s+=buf[i];
			return true;
			// emit (caughtString(QString::fromStdString(str)));
		}
		else
			return false;
	}
};

#endif // _WIN32
#endif
