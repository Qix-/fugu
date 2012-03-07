/**
 * \file
 * \brief A simple console widget
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

#ifndef FGE_CONSOLEWIDGET_H
#define FGE_CONSOLEWIDGET_H

/**
 * A simple console widget.
 */

// #include <QTextEdit>

#include <QKeyEvent>
#include <Qsci/qsciscintilla.h>
#include "fglexer.h"

class CommandLineWidget;

// Adapted from: http://code.google.com/p/qterminalwidget/source/browse/trunk/qconsolewidget.cpp

// Other refs:
// http://www.qtcentre.org/threads/17847-Design-a-commandline-widget-in-Qt4



class ConsoleWidget: public QWidget {
	Q_OBJECT
public:
	ConsoleWidget(QWidget* parent = 0);
	~ConsoleWidget();

	virtual QSize sizeHint(){return QSize(5,5);}

private:
    int fixedPosition;

    QsciScintilla* mConsole;
    CommandLineWidget* mCommandLine;

public slots:
	void processCommand(QString cmd);
	void print(QString p);
	void error(QString p);

    void textChanged();
    void cursorPositionChanged(int line, int index);
    void selectionChanged();
    void linesChanged();
signals:
	void emitCommand(QString command);

protected:
    // void keyPressEvent (QKeyEvent *e);
// public slots:
    // void cursorPositionChanged();
};

class CommandLineWidget: public QsciScintilla {
	Q_OBJECT
public:
	CommandLineWidget(QWidget* parent = 0);

public slots:
	void cursorPositionChanged(int line, int index);
	void selectionChanged();

signals:
	void emitCommand(QString command);

private:
    int fixedPosition;
protected:
	virtual void keyPressEvent(QKeyEvent* e);
};




#endif
