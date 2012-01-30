/**
 * \file
 * \brief Std stream redirection
 * \author ben
  *
 * \cond showlicense
 * \verbatim
 * --------------------------------------------------------------
 *    ___
 *   |  _|___
 *   |  _| . | fg: real-time procedural
 *   |_| |_  | animation and generation
 *       |___| of 3D forms
 *
 *   Copyright (c) 2012 Centre for Electronic Media Art (CEMA)
 *   Monash University, Australia. All rights reserved.
 *
 *   Use of this software is governed by the terms outlined in
 *   the LICENSE file.
 *
 * --------------------------------------------------------------
 * \endverbatim
 * \endcond
 */


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
}

void errcallback( const char* ptr, std::streamsize count, void* _qr )
{
	QRedirector* qr = static_cast<QRedirector*>(_qr);
	QString str = QString::fromAscii(ptr,count);
	qr->_sendErr(str);
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
