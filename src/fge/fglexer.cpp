#include "fglexer.h"
#include <QApplication>

FGLexer::FGLexer()
:QsciLexerLua()
{
	mFont = QFont("Courier New", 8);
	mBoldFont = mFont;
	mBoldFont.setBold(true);
}

FGLexer::~FGLexer(){

}

/*
 * <LexerType name="lua" desc="Lua" ext="">
            <WordsStyle name="DEFAULT" styleID="0" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="COMMENT" styleID="1" fgColor="008000" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="COMMENT LINE" styleID="2" fgColor="008000" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="COMMENT DOC" styleID="3" fgColor="008080" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="LITERALSTRING" styleID="8" fgColor="95004A" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="PREPROCESSOR" styleID="9" fgColor="804000" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="INSTRUCTION WORD" styleID="5" fgColor="0000FF" bgColor="FFFFFF" fontName="" fontStyle="1" fontSize="" keywordClass="instre1" />
            <WordsStyle name="NUMBER" styleID="4" fgColor="FF8000" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="STRING" styleID="6" fgColor="808080" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="CHARACTER" styleID="7" fgColor="808080" bgColor="FFFFFF" fontName="" fontStyle="0" fontSize="" />
            <WordsStyle name="OPERATOR" styleID="10" fgColor="000080" bgColor="FFFFFF" fontName="" fontStyle="1" fontSize="" />
            <WordsStyle name="FUNC1" styleID="13" fgColor="0080C0" bgColor="FFFFFF" fontName="" fontStyle="1" fontSize="" keywordClass="instre2" />
            <WordsStyle name="FUNC2" styleID="14" fgColor="8000FF" bgColor="FFFFFF" fontName="" fontStyle="1" fontSize="" keywordClass="type1" />
            <WordsStyle name="FUNC3" styleID="15" fgColor="0000A0" bgColor="FFFFFF" fontName="" fontStyle="3" fontSize="" keywordClass="type2" />
 *
 *
 */

QColor FGLexer::defaultColor (int style) const {
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
	return Qt::white;
}
