#include "fglexer.h"
#include <QApplication>

FGLexer::FGLexer(QColor bg)
:QsciLexerLua()
,mBackgroundColor(bg)
{
	mFont = QFont("Courier New", 8);
	mBoldFont = mFont;
	mBoldFont.setBold(true);
}

FGLexer::~FGLexer(){

}

QColor FGLexer::defaultColor (int style) const {

	switch (style){
		case Default:
		case Identifier:
			return QColor("#c8c8c8"); break;
		case Comment:
		case LineComment:
			return QColor("#959595"); break;
		case Number: return QColor("#c8c8c8"); break;
		case Keyword: return QColor("#f27000"); break;
		case String:
		case Character: return QColor("#f27000"); break;
		case LiteralString: return QColor("#f27000"); break;
		case Preprocessor: return QColor("#804000"); break;
		case Operator: return QColor("#c8c8c8"); break;
		case UnclosedString: return QColor("#fc0000"); break;
		case BasicFunctions:
		case StringTableMathsFunctions:
		case CoroutinesIOSystemFacilities: return QColor("#f27000"); break;
		case KeywordSet5:
		case KeywordSet6:
		case KeywordSet7:
		case KeywordSet8:
		default:
			return QColor("#c8c8c8"); break;
	}


	// notepad++ style
	/*
	switch (style){
		case Default:
		case Identifier:
			return Qt::black; break;
		case Comment:
		case LineComment:
			return QColor("#008000"); break;
		case Number: return QColor("#FF8000"); break;
		case Keyword: return QColor("#0000FF"); break;
		case String:
		case Character: return QColor("#808080"); break;
		case LiteralString: return QColor("#95004A"); break;
		case Preprocessor: return QColor("#804000"); break;
		case Operator: return QColor("#000080"); break;
		case UnclosedString: return QColor("#800000"); break;
		case BasicFunctions:
		case StringTableMathsFunctions:
		case CoroutinesIOSystemFacilities: return QColor("#0080C0"); break;
		case KeywordSet5:
		case KeywordSet6:
		case KeywordSet7:
		case KeywordSet8:
		default:
			return Qt::black; break;
	}
	*/
}

bool FGLexer::defaultEolFill (int style) const {
	return false;
}

QFont FGLexer::defaultFont (int style) const {
	switch (style){
		case Keyword: return mBoldFont; break;
		default: return mFont; break;
	}
}

QColor FGLexer::defaultPaper (int style) const {
	return mBackgroundColor; // QColor("#272727"); // Qt::white;
}
