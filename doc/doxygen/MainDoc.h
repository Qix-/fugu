/*! \mainpage notitle
 
 \image html fg.png
 
 <CENTER>
 \ref intro &nbsp;&nbsp;
 \ref overview &nbsp;&nbsp;
 \ref download &nbsp;&nbsp;
 \ref start &nbsp;&nbsp;
 \ref license
 </CENTER>
 
 
 
 \section intro Introduction
 
<i>fg</i> is a system for procedurally generating animated geometric forms in real-time. It is currently under heavy development and as such should not be used at all, by anyone, under any circumstances. 
 
 \section overview Overview
 
 The following table lists the frameworks currently included as part of <i>fg</i>, along with a brief description of each.
 
 <table class="modules" border="0" cellspacing="0" cellpadding="0">
 <TR>
 <th class="modules" WIDTH="5%"><B>Framework</B></th>
 <th class="modules"><B>Description</B></th>
 </TR>
 <TR>
 <TD class="modules">\ref fg</TD>
 <TD class="modules">\copydoc fg</TD>
 </TR>
 </table>
 
 
 
 \section download Download
 
 Check the code out of the Bitbucket Mercurial repository:
 
 \code
 https://bitbucket.org/bpio/fg
 \endcode
 
 See Ben for login details.
 
 
 \section start Getting Started
 
 Install the requirements:
 <OL>
 <LI>Cmake 2.8 http://www.cmake.org/
	\n
	<i>fg</i> uses CMake to build. As such, it should be possible to output project files for Xcode, Visual Studio, etc. Some build scripts are provided for some of these, to generate other projects you will need to write your own scripts (patches welcome!). 
 </LI>
 <LI>xcode for Mac </LI>
 <LI>Eclipse/MinGW for Windows </LI>
 </OL>
 
 Building <i>fg</i>:
 <UL>
  <LI>Mac</LI>
 	<OL>
 	<LI>./xcode-build.sh</LI>
 	<LI>open build/xcode/FG.xcodeproj</LI>
	<LI>Select "install" target and run</LI>
	<LI>Open a terminal, cd to fg/bin/fgv and run "./fgv tests/basic5" for example</LI>
 	</OL>
  <LI>Windows</LI>
	<OL>
	<LI>Switch to Mac</LI>
	<LI>Follow instruction steps above :-)</LI>
	</OL>
  </UL> 
 
 
 \section license License
 
 <i>fg</i>\n
 Copyright (c) 2011 Centre for Electronic Media Art (CEMA)\n
 Monash University, Australia. All rights reserved.

 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation files
 (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software,
 and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.
 
 Any person wishing to distribute modifications to the Software is
 requested to send the modifications to the original developer so that
 they can be incorporated into the canonical version.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 */
