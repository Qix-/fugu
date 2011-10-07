#ifndef FGLEXER_H
#define FGLEXER_H

#include <QColor>
#include <QFont>
#include <Qsci/qscilexerlua.h>

/**
 * Overrides the horrible default syntax highlighting to
 * the nicer colours used in notepad++.
 */
class FGLexer: public QsciLexerLua {
public:
	FGLexer(QColor bg = QColor("#272727"));
	~FGLexer();

	QColor defaultColor (int style) const;
	bool defaultEolFill (int style) const;
	QFont defaultFont (int style) const;
	QColor defaultPaper (int style) const;

protected:
	QFont mFont;
	QFont mBoldFont;
	QColor mBackgroundColor;
};

#endif
