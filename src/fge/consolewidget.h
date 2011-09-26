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

// TODO: redirect std::cout, std::cerr to it
// see e.g., http://www.qtforum.org/article/24554/displaying-std-cout-in-a-text-box.html
// or http://lists.qt.nokia.com/pipermail/qt-interest/2011-January/030985.html

class ConsoleWidget: public QWidget {
	Q_OBJECT
public:
	ConsoleWidget(QWidget* parent = 0);
	~ConsoleWidget();
private:
    int fixedPosition;

    QsciScintilla* mConsole;
    // QsciScintilla* mCommandLine;
    CommandLineWidget* mCommandLine;

public slots:
	void processCommand(QString cmd);

    void textChanged();
    void cursorPositionChanged(int line, int index);
    void selectionChanged();
    void linesChanged();
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
