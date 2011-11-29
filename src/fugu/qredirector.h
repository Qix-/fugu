#ifndef FUGU_QREDIRECTOR_H
#define FUGU_QREDIRECTOR_H

#include "stdredirector.h"
#include <QThread>
#include <QString>

class QRedirectorImpl;

/*
 * Captures stdout and stderr and emits the contents.
 */
class QRedirector: public QThread {
	Q_OBJECT

public:
	QRedirector(QObject* parent);
	~QRedirector();

	void run();

signals:
	void out(QString str);
	void err(QString str);

public slots:
	void _update(); // update if loop based (internal)

public:
	// activate the signals (internal use only)
	void _sendOut(QString str);
	void _sendErr(QString str);

protected:
	QRedirectorImpl* pImpl;

	StdRedirector<>* mStdOutRedirector;
	StdRedirector<>* mStdErrRedirector;

protected:
	void _run(); // internal use only
};

#endif
