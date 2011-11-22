#include "qredirector.h"

#ifdef _WIN32
	#include "qredirectorimpl_win32.h"
#else
	#include "qredirectorimpl_posix.h"
#endif

void outcallback( const char* ptr, std::streamsize count, void* _qr )
{
	QRedirector* qr = static_cast<QRedirector*>(_qr);
	QString str = QString::fromAscii(ptr,count);
	qr->_sendOut(str);

	// (void) count;
	/*
	QsciScintilla* p = static_cast< QsciScintilla* >( console );
	QString str = QString::fromAscii(ptr,count);
	p->append( str );
	p->setCursorPosition(p->lines()-1,0);
	 */
}

void errcallback( const char* ptr, std::streamsize count, void* _qr )
{
	QRedirector* qr = static_cast<QRedirector*>(_qr);
	QString str = QString::fromAscii(ptr,count);
	qr->_sendErr(str);

	/*
	QsciScintilla* p = static_cast< QsciScintilla* >( console );
	int linesbefore = p->lines();
	QString str = QString::fromAscii(ptr,count);
	p->append( str );
	int linesafter = p->lines();
	//p->setSelection(linesbefore,0,linesafter-1,p->lineLength(linesafter-1));
	//p->selectAll(false);
	p->setCursorPosition(p->lines()-1,0);
	 */
}

QRedirector::QRedirector(QObject* p):QThread(p){

}

QRedirector::~QRedirector(){
	delete pImpl;
	delete mStdOutRedirector;
	delete mStdErrRedirector;
}

void QRedirector::run(){
	mStdOutRedirector = new StdRedirector<>( std::cout, outcallback, this );
	mStdErrRedirector = new StdRedirector<>( std::cerr, errcallback, this );
	pImpl = new QRedirectorImpl();
	_run();
}

void QRedirector::_sendOut(QString str){
	emit out(str);
}
void QRedirector::_sendErr(QString str){
	emit err(str);
}

#ifdef _WIN32
#include <QTimer>
void QRedirector::_run(){
	QTimer* t = new QTimer;
	// t->moveToThread(this);
	t->setSingleShot(false);
	// Qt::DirectConnection idea from http://www.qtforum.org/article/18469/qthread-and-qtimer-qt4.html
	connect(t, SIGNAL(timeout()), this, SLOT(_update()), Qt::DirectConnection);
	t->start(100);
	exec();
	delete t;
}

void QRedirector::_update(){
	std::string s;
	if (pImpl->nextOut(s)){
		emit out(QString::fromStdString(s));
	}
}
#else
void QRedirector::_run(){
	connect(pImpl,SIGNAL(out(QString)),this,SLOT(out(QString)));
	connect(pImpl,SIGNAL(err(QString)),this,SLOT(err(QString)));
	exec();
}

void QRedirector::_update(){/* n/a */}
#endif
