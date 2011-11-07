#include "fglexer.h"
#include <QApplication>
#include <QPalette>

#include <iostream>

FGLexer::FGLexer(QObject* parent, QColor bg)
:QsciLexerLua(parent)
,mBackgroundColor(bg)
//,mParent(NULL)
{
	// QApplication::desktop()->physicalDpiX();
	// QApplication::desktop()->physicalDpiY();
	mFont = QFont("Courier New", QApplication::font().pointSize());
	mFont.setStyleHint(QFont::TypeWriter);
	mBoldFont = mFont;
	mBoldFont.setBold(true);

	setColor(QColor("#ff00ff"), -1);
}

FGLexer::~FGLexer(){

}

void FGLexer::inheritColoursFrom(QWidget* p){
	//mParent = p;
}

QColor FGLexer::defaultColor (int style) const {

	if (style==KeywordSet5){
		std::cout << "keyword set 5: " << ((QsciLexerLua*)(this))->keywords(style) << "\n";
	}
	switch (style){
		case KeywordSet5:
		case KeywordSet6:
		case KeywordSet7:
		case KeywordSet8:
			return QColor("#ff00ff");
		case Default:
		case Identifier:
			return QColor("#c8c8c8");
		case Comment:
		case LineComment:
			return QColor("#959595");
		case Number:
			return QColor("#c8c8c8");
		case Keyword:
			return QColor("#f27000");
		case String:
		case Character:
			return QColor("#f27000");
		case LiteralString:
			return QColor("#f27000");
		case Preprocessor:
			return QColor("#804000");
		case Operator:
			return QColor("#c8c8c8");
		case UnclosedString:
			return QColor("#fc0000");
		case BasicFunctions:
		case StringTableMathsFunctions:
		case CoroutinesIOSystemFacilities:
			return QColor("#f27000");
		default:
			return QColor("#c8c8c8");
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
		case Keyword: return mBoldFont;
		case KeywordSet5: return mBoldFont;
		default: return mFont;
	}
}

QColor FGLexer::defaultPaper (int style) const {
	/*
	if (mParent){
		return mParent->palette().color(QPalette::Window);
	}
	else
	*/
	return mBackgroundColor; // QColor("#272727"); // Qt::white;
}

const char* FGLexer::keywords(int set) const {
	if (set==KeywordSet5)
		return "mesh";

	// else
	return QsciLexerLua::keywords(set);
}
