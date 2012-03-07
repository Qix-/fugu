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


#include "consolewidget.h"

#include <QKeyEvent>
#include <QVBoxLayout>
#include <Qsci/qscilexer.h>

#include <iostream>
//#include "redirect.h"

class ConsoleLexer: public QsciLexer {
public:
	ConsoleLexer(const FGLexer& f):QsciLexer(NULL){
		mFont = f.defaultFont(FGLexer::Default);
		mBackgroundColor = f.defaultPaper(FGLexer::Default);
	}

	QColor defaultColor (int style) const {return QColor("#c8c8c8");}
	bool defaultEolFill (int style) const {return false;}
	QFont defaultFont (int style) const {return mFont; }
	QColor defaultPaper (int style) const {return mBackgroundColor; }
	virtual const char* language() const {return "fgconsole";}
	virtual QString description(int) const {return "fgconsole term";}
protected:
	QFont mFont;
	QColor mBackgroundColor;
};


ConsoleWidget::ConsoleWidget(QWidget *parent)
: QWidget(parent)
{
    mConsole = new QsciScintilla();
    //FGLexer fg(mConsole); // ,QColor("#101010"));
    // mConsole->setLexer(new ConsoleLexer(fg));
    mConsole->setLexer(new FGLexer(mConsole));
    mConsole->setObjectName("console");
    mConsole->setTabWidth(2);
    mConsole->setWrapMode(QsciScintilla::WrapCharacter);
    mConsole->setCaretForegroundColor(QColor("#959595"));
	//mConsole->setMarginsBackgroundColor(QColor("#101010"));
	//mConsole->setMarginsForegroundColor(QColor("#959595"));

    mConsole->setMarginsBackgroundColor(QColor("#272727"));
    mConsole->setMarginsForegroundColor(QColor("#959595"));

	mConsole->setMarginWidth(1,0); // disable line number margin
	mConsole->setMarginWidth(2,8); // a little margin
	mConsole->setReadOnly(true);

	/*
    mCommandLine = new CommandLineWidget();

    // hook up command line signals...
    connect(mCommandLine, SIGNAL(emitCommand(QString)), this, SLOT(processCommand(QString)));
	*/

    QVBoxLayout* qvb = new QVBoxLayout();
    qvb->addWidget(mConsole);
    // qvb->addWidget(mCommandLine);
    qvb->setContentsMargins(0,0,0,0);
    setLayout(qvb);

    // mStdOutRedirector = new StdRedirector<>( std::cout, outcallback, mConsole );
    // mStdErrRedirector = new StdRedirector<>( std::cerr, errcallback, mConsole );

    mConsole->resize(mConsole->minimumSize());
}

ConsoleWidget::~ConsoleWidget()
{
}

void ConsoleWidget::processCommand(QString cmd){

	// Echo command
	mConsole->append(QString("> ") + cmd + "\n");
	// Scroll to bottom
	mConsole->setCursorPosition(mConsole->lines()-1,0);
	// mConsole->ensureLineVisible(mConsole->lines()-1);

	// send the command out
	emit emitCommand(cmd);
}

void ConsoleWidget::print(QString p){
	mConsole->append(p);
	// Scroll to bottom
	mConsole->setCursorPosition(mConsole->lines()-1,0);
}

void ConsoleWidget::error(QString p){
	mConsole->append(p);
	// Scroll to bottom
	mConsole->setCursorPosition(mConsole->lines()-1,0);
}

/*
void ConsoleWidget::OnChildStarted()
{
}

void QConsoleWidget::OnChildStdOutWrite(QString szOutput)
{
#ifdef Q_OS_WIN32
    QString content;

    if (fixedPosition != 0) {
        content = szOutput.right(szOutput.count() - szOutput.indexOf('\n') - 1);
    } else {
        content = szOutput;
    }

    if (content.startsWith("\f\r\n")) {
        clear();
        insertPlainText(content.right(content.count() - 3));
    } else {
        insertPlainText(content);
    }
#else
    insertPlainText(szOutput);
#endif
    fixedPosition = textCursor().position();
}

void QConsoleWidget::OnChildStdErrWrite(QString szOutput)
{
    append(szOutput);
    fixedPosition = textCursor().position();
}

void QConsoleWidget::OnChildTerminate()
{
    //exit(1);
}
*/

/*
void ConsoleWidget::keyPressEvent(QKeyEvent *event)
{
    bool accept;
    int key = event->key();
    if (key == Qt::Key_Backspace || event->key() == Qt::Key_Left) {
        accept = textCursor().position() > fixedPosition;
    } else if (key == Qt::Key_Return) {
        accept = false;
        int count = toPlainText().count() - fixedPosition;
        QString cmd = toPlainText().right(count);
        // redirect->WriteChildStdIn(cmd + "\n");
    } else if (key == Qt::Key_Up) {
        accept = false;
    } else {
        accept = textCursor().position() >= fixedPosition;
    }

    if (accept) {
        QTextEdit::keyPressEvent(event);
    }
}

void ConsoleWidget::cursorPositionChanged()
{
    if (textCursor().position() < fixedPosition) {
        textCursor().setPosition(fixedPosition);
    }
}
*/

void ConsoleWidget::textChanged(){
	/*
	else if (mCommandLine->text(0).count() < fixedPosition){
		mCommandLine->clear();
		//mCommandLine->setText(">> ");
		mCommandLine->setCursorPosition(0,0);
	}
	*/
}

void ConsoleWidget::cursorPositionChanged(int line, int index){
	/*
	std::cerr << line << "_" << index << "\n";
	bool t = false;
	if (line!=0){
		line = 0;
		t = true;
	}
	if (index < fixedPosition){
		index = fixedPosition;
		t = true;
	}
	if (t) mCommandLine->setCursorPosition(line,index);
	*/
}

void ConsoleWidget::selectionChanged(){
	int fromL, toL, fromI, toI;
	mCommandLine->getSelection(&fromL, &fromI, &toL, &toI);
	if (fromL!=-1){
		if (fromL!=0 or toL!=0){
			fromL = 0;
			toL = 0;
			mCommandLine->setSelection(fromL,fromI,toL,toI);
		}
	}
}

void ConsoleWidget::linesChanged(){
	/*
	std::cerr << "\"" << mCommandLine->text(0).toStdString() << "\"\n";

	// only allow one line of text...?
	if (mCommandLine->lines()>1){
		int count = mCommandLine->text(0).count() - fixedPosition;
		QString cmd = mCommandLine->text(0).right(count);
		if (not cmd.isEmpty()){
			mConsole->append(cmd);
		}
		mCommandLine->clear();

		//mCommandLine->selectAll(); //  clear();
		//mCommandLine->removeSelectedText(); // selectAll();

		//mCommandLine->setText(">> ");
		mCommandLine->setCursorPosition(0,0);
	}*/

}

// void ConsoleWidget::keyPressEvent (QKeyEvent *event){}

CommandLineWidget::CommandLineWidget(QWidget* parent):QsciScintilla(parent){
	setObjectName("commandline");

	FGLexer* lex = new FGLexer(this); // , QColor("#101010"));
	// lex->inheritColoursFrom(this);
	setLexer(lex);

	setText("> ");
	fixedPosition = 2;
	setCursorPosition(0,fixedPosition);

	connect(this,SIGNAL(selectionChanged()),this, SLOT(selectionChanged()));
	connect(this,SIGNAL(cursorPositionChanged(int,int)),this, SLOT(cursorPositionChanged(int,int)));

    setTabWidth(2);
    setMarginWidth(1,0); // disable line number margin
    setMarginWidth(2,8); // a little margin
    setCaretForegroundColor(QColor("#959595"));
    //setMarginsBackgroundColor(QColor("#101010"));
    //setMarginsForegroundColor(QColor("#959595"));

    setMarginsBackgroundColor(QColor("#272727"));
    setMarginsForegroundColor(QColor("#959595"));

    QFontMetrics fm(lexer()->defaultFont());
    int textHeightInPixels = fm.height();
    setMaximumSize(maximumWidth(), textHeightInPixels);
    setMinimumSize(0, textHeightInPixels);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
}

void CommandLineWidget::keyPressEvent(QKeyEvent* event){
	bool accept;
	int key = event->key();
	if (key == Qt::Key_Backspace or event->key() == Qt::Key_Left) {
		int cursorPos, cursorLine;
		getCursorPosition(&cursorLine,&cursorPos);
		accept = cursorPos > fixedPosition;
	} else if (key == Qt::Key_Return or key == Qt::Key_Enter) {
		accept = false;
		int count = text(0).count() - fixedPosition;
		QString cmd = text(0).right(count);
		if (not cmd.isEmpty()){
			emit emitCommand(cmd);
		}
		setText("> ");
		setCursorPosition(0,fixedPosition);
		// redirect->WriteChildStdIn(cmd + "\n");
	} else if (key == Qt::Key_Up) {
		accept = false;
	} else {
		int cursorPos, cursorLine;
		getCursorPosition(&cursorLine,&cursorPos);
		accept = cursorPos >= fixedPosition;
	}

	if (accept) {
		QsciScintilla::keyPressEvent(event);
		// ((QWidget*)mCommandLine)->keyPressEvent(event);
	}
}

void CommandLineWidget::cursorPositionChanged(int line, int index){
	bool t = false;
	if (line!=0){
		line = 0;
		t = true;
	}
	if (index < fixedPosition){
		index = fixedPosition;
		t = true;
	}
	if (t) setCursorPosition(line,index);
}

void CommandLineWidget::selectionChanged(){
	int fromL, toL, fromI, toI;
	getSelection(&fromL, &fromI, &toL, &toI);
	if (fromL!=-1){
		if (fromL!=0 or toL!=0 or fromI<fixedPosition or toI<fixedPosition){
			fromL = 0;
			toL = 0;
			fromI = std::max(fromI,fixedPosition);
			toI = std::max(toI,fixedPosition);
			setSelection(fromL,fromI,toL,toI);
		}
	}
}
