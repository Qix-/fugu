// hacked from a few examples..


/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by David Bitseff                                   *
 *   dbitsef@zipcon.net                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

 #include <QtGui>

 #include "highlighter.h"

 Highlighter::Highlighter(QTextDocument *parent)
     : QSyntaxHighlighter(parent)
 {

	 /**
	 	 * This is the syntax highlighter copied from the example in the qt4 docs,
	 	 * and modified look for lua style text.
	 	 *
	 	 * Each rule is applied in order to the text in a given block.  The last
	 	 * rule to match wins.
	 	 */
	 	HighlightingRule rule;

	 	constantFormat.setForeground(Qt::darkGreen);
	 	rule.pattern = QRegExp("[-+]?(?:(?:\\d+\\.\\d+)|(?:\\.\\d+)|(?:\\d+\\.?))");
	 	rule.format = constantFormat;
	 	highlightingRules.append(rule);

	 	classFormat.setForeground(Qt::darkMagenta);
	 	rule.pattern = QRegExp("\\b[A-Za-z][A-Za-z0-9_]*\\b");
	 	rule.format = classFormat;
	 	highlightingRules.append(rule);

	 	functionFormat.setForeground(Qt::blue);
	 	rule.pattern = QRegExp("\\b[A-Za-z0-9_]+ *(?=\\()");
	 	rule.format = functionFormat;
	 	highlightingRules.append(rule);

	 	keywordFormat.setForeground(Qt::darkBlue);
	 	keywordFormat.setFontWeight(QFont::Bold);
	 	QStringList keywordPatterns;
	 	keywordPatterns
	 			<< "\\bdo\\b" << "\\bend\\b" << "\\bfor\\b" << "\\bwhile\\b"
	 			<< "\\bif\\b" << "\\bthen\\b" << "\\belse\\b" << "\\belseif\\b"
	 			<< "\\brepeat\\b" << "\\buntil\\b" << "\\bfunction\\b" << "\\blocal\\b"
	 			<< "\\breturn\\b" << "\\bin\\b" << "\\bbreak\\b" << "\\bnot\\b"
	 			<< "\\bnil\\b" << "\\band\\b" << "\\bor\\b" << "\\btrue\\b"
	 			<< "\\bfalse\\b";
	 	foreach (QString pattern, keywordPatterns)
	 	{
	 		rule.pattern = QRegExp(pattern);
	 		rule.format = keywordFormat;
	 		highlightingRules.append(rule);
	 	}

	 	quotationFormat.setForeground(Qt::darkGreen);
	 	rule.pattern = QRegExp("\"[^\"]*\"");
	 	rule.format = quotationFormat;
	 	highlightingRules.append(rule);

	 	rule.pattern = QRegExp("'[^']*'");
	 	rule.format = quotationFormat;
	 	highlightingRules.append(rule);

	 	singleLineCommentFormat.setForeground(Qt::gray);
	 	singleLineCommentFormat.setFontItalic(true);
	 	rule.pattern = QRegExp("--(?!\\[\\[)[^\n]*");
	 	rule.format = singleLineCommentFormat;
	 	highlightingRules.append(rule);

	 	multiLineCommentFormat.setForeground(Qt::gray);
	 	multiLineCommentFormat.setFontItalic(true);

	 	commentStartExpression = QRegExp("--\\[\\[");
	 	commentEndExpression = QRegExp("--\\]\\]");

	 /*
     HighlightingRule rule;

     keywordFormat.setForeground(Qt::darkBlue);
     keywordFormat.setFontWeight(QFont::Bold);
     QStringList keywordPatterns;
     keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                     << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                     << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                     << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                     << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                     << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                     << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                     << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                     << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                     << "\\bvoid\\b" << "\\bvolatile\\b";
     foreach (const QString &pattern, keywordPatterns) {
         rule.pattern = QRegExp(pattern);
         rule.format = keywordFormat;
         highlightingRules.append(rule);
     }

     classFormat.setFontWeight(QFont::Bold);
     classFormat.setForeground(Qt::darkMagenta);
     rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
     rule.format = classFormat;
     highlightingRules.append(rule);

     singleLineCommentFormat.setForeground(Qt::red);
     rule.pattern = QRegExp("//[^\n]*");
     rule.format = singleLineCommentFormat;
     highlightingRules.append(rule);

     multiLineCommentFormat.setForeground(Qt::red);

     quotationFormat.setForeground(Qt::darkGreen);
     rule.pattern = QRegExp("\".*\"");
     rule.format = quotationFormat;
     highlightingRules.append(rule);

     functionFormat.setFontItalic(true);
     functionFormat.setForeground(Qt::blue);
     rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
     rule.format = functionFormat;
     highlightingRules.append(rule);

	  */
     // commentStartExpression = QRegExp("/\\*");
     // commentEndExpression = QRegExp("\\*/");
 }

 void Highlighter::highlightBlock(const QString &text)
 {
     foreach (const HighlightingRule &rule, highlightingRules) {
         QRegExp expression(rule.pattern);
         int index = expression.indexIn(text);
         while (index >= 0) {
             int length = expression.matchedLength();
             setFormat(index, length, rule.format);
             index = expression.indexIn(text, index + length);
         }
     }
     setCurrentBlockState(0);

     int startIndex = 0;
     if (previousBlockState() != 1)
         startIndex = commentStartExpression.indexIn(text);

     while (startIndex >= 0) {
         int endIndex = commentEndExpression.indexIn(text, startIndex);
         int commentLength;
         if (endIndex == -1) {
             setCurrentBlockState(1);
             commentLength = text.length() - startIndex;
         } else {
             commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
         }
         setFormat(startIndex, commentLength, multiLineCommentFormat);
         startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
     }
 }

 /*
 void Highlighter::highlightBlock(const QString &text)
 {
 	foreach (HighlightingRule rule, highlightingRules)
 	{
 		QRegExp expression(rule.pattern);
 		int index = text.indexOf(expression);
 		while (index >= 0)
 		{
 			int length = expression.matchedLength();
 			setFormat(index, length, rule.format);
 			index = text.indexOf(expression, index + length);
 		}
 	}
 }
*/
