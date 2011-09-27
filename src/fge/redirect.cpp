#include "redirect.h"

#ifdef _WIN32

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>

#include <QThread>



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

class MyThread : public QThread
 {
 public:
     void run();
 };

 void MyThread::run()
 {
	HANDLE rd,wr;
	if (!CreatePipe(&rd,&wr,NULL,0))
		std::cout << "Error creating pipe.\n";
	RedirectConsole(wr);
	//std::cout << "cout\n";
	//printf("printf\n");
	char buf[100];
	while (true)	{
		DWORD readBytes=0;
		if (!ReadFile(rd, buf, 99, &readBytes, 0))
			break;
		buf[99] = 0;

		if (readBytes>0){
			std::string str;
			for (DWORD i=0;i<readBytes;i++)
				str+=buf[i];
			std::cout << str.c_str();
			// MessageBoxA(0,str.c_str(),"",0);
		}
		else
			break;
	}
 }

void redirectConsoleOutput(){
	MyThread* th = new MyThread;
	th->start();

	// the thread object remains in memory...
}

#else // end _WIN32

#include <stdexcept>
void redirectConsoleOutput(){
	throw(std::runtime_error("Sorry, not yet implemented!"));
}





#endif
