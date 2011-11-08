#include "fglexer.h"

#include <QApplication>
#include <QPalette>

/*
#include <QApplication>
#include <QPalette>

#include <iostream>

FGLexer::FGLexer(QObject* parent, QColor bg)
:FGLexer(parent)
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
		std::cout << "keyword set 5: " << ((FGLexer*)(this))->keywords(style) << "\n";
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

	//if (mParent){
//		return mParent->palette().color(QPalette::Window);
//	}

	return mBackgroundColor; // QColor("#272727"); // Qt::white;
}

const char* FGLexer::keywords(int set) const {
	if (set==KeywordSet5)
		return "mesh";

	// else
	return FGLexer::keywords(set);
}


*/


// This module implements the FGLexer class.
//
// Copyright (c) 2011 Riverbank Computing Limited <info@riverbankcomputing.com>
//
// This file is part of QScintilla.
//
// This file may be used under the terms of the GNU General Public
// License versions 2.0 or 3.0 as published by the Free Software
// Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
// included in the packaging of this file.  Alternatively you may (at
// your option) use any later version of the GNU General Public
// License if such license has been publicly approved by Riverbank
// Computing Limited (or its successors, if any) and the KDE Free Qt
// Foundation. In addition, as a special exception, Riverbank gives you
// certain additional rights. These rights are described in the Riverbank
// GPL Exception version 1.1, which can be found in the file
// GPL_EXCEPTION.txt in this package.
//
// Please review the following information to ensure GNU General
// Public Licensing requirements will be met:
// http://trolltech.com/products/qt/licenses/licensing/opensource/. If
// you are unsure which license is appropriate for your use, please
// review the following information:
// http://trolltech.com/products/qt/licenses/licensing/licensingoverview
// or contact the sales department at sales@riverbankcomputing.com.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>

// The ctor.
FGLexer::FGLexer(QObject *parent)
    : QsciLexer(parent),
      fold_compact(true),
      mFuguKeywords(NULL)
{
	mFont = QFont("Consolas", QApplication::font().pointSize()); // "Courier New"
	mFont.setStyleHint(QFont::TypeWriter);
	mBoldFont = mFont;
	mBoldFont.setBold(true);
}


// The dtor.
FGLexer::~FGLexer()
{
}


// Returns the language name.
const char *FGLexer::language() const
{
    return "Lua";
}


// Returns the lexer name.
const char *FGLexer::lexer() const
{
    return "lua";
}


// Return the set of character sequences that can separate auto-completion
// words.
QStringList FGLexer::autoCompletionWordSeparators() const
{
    QStringList wl;

    wl << ":" << ".";

    return wl;
}


// Return the list of characters that can start a block.
const char *FGLexer::blockStart(int *style) const
{
    if (style)
        *style = Operator;

    return "";
}


// Return the style used for braces.
int FGLexer::braceStyle() const
{
    return Operator;
}


// Returns the foreground colour of the text for a style.
/*
QColor FGLexer::defaultColor(int style) const
{
    switch (style)
    {
    case Default:
        return QColor(0x00,0x00,0x00);

    case Comment:
    case LineComment:
        return QColor(0x00,0x7f,0x00);

    case Number:
        return QColor(0x00,0x7f,0x7f);

    case Keyword:
    case BasicFunctions:
    case StringTableMathsFunctions:
    case CoroutinesIOSystemFacilities:
        return QColor(0x00,0x00,0x7f);

    case String:
    case Character:
    case LiteralString:
        return QColor(0x7f,0x00,0x7f);

    case Preprocessor:
        return QColor(0x7f,0x7f,0x00);

    case Operator:
    case Identifier:
        break;
    }

    return QsciLexer::defaultColor(style);
}
*/

QColor FGLexer::defaultColor (int style) const {

	//if (style==KeywordSet5){
//		std::cout << "keyword set 5: " << ((FGLexer*)(this))->keywords(style) << "\n";
//	}
	switch (style){
		case KeywordSet5:
		case KeywordSet6:
		case KeywordSet7:
		case KeywordSet8:
		case BasicFunctions:
		case StringTableMathsFunctions:
		case CoroutinesIOSystemFacilities:
			return QColor("#2773e2"); //  QColor("#87ceeb"); // QColor("#ff00ff");
		case Default:
			return QColor("#c8c8c8");
		case Identifier:
			return QColor("#2773e2");
		case Comment:
		case LineComment:
			return QColor("#959595");
		case Number:
			return QColor("#c8c8c8");
		case Keyword:
			return QColor("#dc9d68"); // QColor("#f27000");
		case String:
		case Character:
			// return QColor("#87ceeb"); // Color("#f27000");
		case LiteralString:
			// return QColor("#f27000");
			return QColor("#87ceeb");
		case Preprocessor:
			return QColor("#804000");
		case Operator:
			return QColor("#dc9d68"); // QColor("#c8c8c8");
		case UnclosedString:
			return QColor("#eb5684"); // QColor("#fc0000");

		default:
			return QColor("#c8c8c8");
	}

}


// Returns the end-of-line fill for a style.
bool FGLexer::defaultEolFill(int style) const
{
    if (style == Comment || style == UnclosedString)
        return true;

    return QsciLexer::defaultEolFill(style);
}

// Returns the font of the text for a style.
QFont FGLexer::defaultFont(int style) const
{
	switch (style){
		case Keyword:
		case KeywordSet5:
		case Operator:
			return mBoldFont;
		default: return mFont; //  QsciLexer::defaultFont(style);
	}
}


// Returns the set of keywords.
const char *FGLexer::keywords(int set) const
{
    if (set == 1)
        // Keywords.
        return
            "and break do else elseif end false for function if "
            "in local nil not or repeat return then true until "
            "while";

    if (set == 2)
        // Basic functions.
        return
            "_ALERT _ERRORMESSAGE _INPUT _PROMPT _OUTPUT _STDERR "
            "_STDIN _STDOUT call dostring foreach foreachi getn "
            "globals newtype rawget rawset require sort tinsert "
            "tremove "

            "G getfenv getmetatable ipairs loadlib next pairs "
            "pcall rawegal rawget rawset require setfenv "
            "setmetatable xpcall string table math coroutine io "
            "os debug";

    if (set == 3)
        // String, table and maths functions.
        return
            "abs acos asin atan atan2 ceil cos deg exp floor "
            "format frexp gsub ldexp log log10 max min mod rad "
            "random randomseed sin sqrt strbyte strchar strfind "
            "strlen strlower strrep strsub strupper tan "

            "string.byte string.char string.dump string.find "
            "string.len string.lower string.rep string.sub "
            "string.upper string.format string.gfind string.gsub "
            "table.concat table.foreach table.foreachi table.getn "
            "table.sort table.insert table.remove table.setn "
            "math.abs math.acos math.asin math.atan math.atan2 "
            "math.ceil math.cos math.deg math.exp math.floor "
            "math.frexp math.ldexp math.log math.log10 math.max "
            "math.min math.mod math.pi math.rad math.random "
            "math.randomseed math.sin math.sqrt math.tan";

    if (set == 4)
        // Coroutine, I/O and system facilities.
        return
            "openfile closefile readfrom writeto appendto remove "
            "rename flush seek tmpfile tmpname read write clock "
            "date difftime execute exit getenv setlocale time "

            "coroutine.create coroutine.resume coroutine.status "
            "coroutine.wrap coroutine.yield io.close io.flush "
            "io.input io.lines io.open io.output io.read "
            "io.tmpfile io.type io.write io.stdin io.stdout "
            "io.stderr os.clock os.date os.difftime os.execute "
            "os.exit os.getenv os.remove os.rename os.setlocale "
            "os.time os.tmpname";

    if (set == 5)
    	// FG keywords
    	return mFuguKeywords; // "mesh icosahedron";

    return 0;
}


// Returns the user name of a style.
QString FGLexer::description(int style) const
{
    switch (style)
    {
    case Default:
        return tr("Default");

    case Comment:
        return tr("Comment");

    case LineComment:
        return tr("Line comment");

    case Number:
        return tr("Number");

    case Keyword:
        return tr("Keyword");

    case String:
        return tr("String");

    case Character:
        return tr("Character");

    case LiteralString:
        return tr("Literal string");

    case Preprocessor:
        return tr("Preprocessor");

    case Operator:
        return tr("Operator");

    case Identifier:
        return tr("Identifier");

    case UnclosedString:
        return tr("Unclosed string");

    case BasicFunctions:
        return tr("Basic functions");

    case StringTableMathsFunctions:
        return tr("String, table and maths functions");

    case CoroutinesIOSystemFacilities:
        return tr("Coroutines, i/o and system facilities");

    case KeywordSet5:
        return tr("User defined 1");

    case KeywordSet6:
        return tr("User defined 2");

    case KeywordSet7:
        return tr("User defined 3");

    case KeywordSet8:
        return tr("User defined 4");
    }

    return QString();
}


// Returns the background colour of the text for a style.
QColor FGLexer::defaultPaper(int style) const
{
	return QColor("#272727");

	/*

    switch (style)
    {
    case Comment:
        return QColor(0xd0,0xf0,0xf0);

    case LiteralString:
        return QColor(0xe0,0xff,0xff);

    case UnclosedString:
        return QColor(0xe0,0xc0,0xe0);

    case BasicFunctions:
        return QColor(0xd0,0xff,0xd0);

    case StringTableMathsFunctions:
        return QColor(0xd0,0xd0,0xff);

    case CoroutinesIOSystemFacilities:
        return QColor(0xff,0xd0,0xd0);
    }

    return QsciLexer::defaultPaper(style);
    */
}


// Refresh all properties.
void FGLexer::refreshProperties()
{
    setCompactProp();
}


// Read properties from the settings.
bool FGLexer::readProperties(QSettings &qs,const QString &prefix)
{
    int rc = true;

    fold_compact = qs.value(prefix + "foldcompact", true).toBool();

    return rc;
}


// Write properties to the settings.
bool FGLexer::writeProperties(QSettings &qs,const QString &prefix) const
{
    int rc = true;

    qs.setValue(prefix + "foldcompact", fold_compact);

    return rc;
}


// Return true if folds are compact.
bool FGLexer::foldCompact() const
{
    return fold_compact;
}


// Set if folds are compact.
void FGLexer::setFoldCompact(bool fold)
{
    fold_compact = fold;

    setCompactProp();
}


// Set the "fold.compact" property.
void FGLexer::setCompactProp()
{
    emit propertyChanged("fold.compact",(fold_compact ? "1" : "0"));
}

