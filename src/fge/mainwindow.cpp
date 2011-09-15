#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
{
	setWindowTitle(tr("fugu"));

	setupFileMenu();
	setupHelpMenu();

	mEditors = new QTabWidget();
	setCentralWidget(mEditors);

	// mEditors->setTabShape(QTabWidget::Triangular); // echk! ugly!
	// mEditors->setTabsClosable(true);
	mEditors->setDocumentMode(true);
	mEditors->setMovable(true);
	mEditors->setUsesScrollButtons(false);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About fugu"),
			tr("<p><b>fugu</b> is software for interactive scripting of " \
					"procedural geometry.</p>"));
}

void MainWindow::newFile()
{
	// editor->clear();
	newEditor();
}

void MainWindow::openFile(const QString &path)
{
	QString fileName = path;

	if (fileName.isNull())
		fileName = QFileDialog::getOpenFileName(this,
				tr("Open File"), "", "fugu scripts (*.lua)");

	if (!fileName.isEmpty()) {
		newEditor(new QFile(fileName));
	}
}

void MainWindow::saveFile(){
	// Save the currently active script

}


void MainWindow::newEditor(QFile* file)
{
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);

	// text editor
	// see the Qt CodeEditor example for line numbers, and line highlighting
	QTextEdit* editor = new QTextEdit;
	editor->setFont(font);
	editor->setTabStopWidth(20);
	Highlighter* highlighter = new Highlighter(editor->document());

	if (file==NULL){
		// Open a blank editor
		int t = mEditors->addTab(editor, "<new>");
	}
	else {
		if (file->open(QFile::ReadOnly | QFile::Text)){
			QFileInfo fi(*file);
			// TODO: Store the file pointer with the editor (or somewhere..)
			// Open an editor with the contents of fileName
			int t = mEditors->addTab(editor, fi.fileName());
			editor->setPlainText(file->readAll());
		}
	}
	mEditors->setCurrentWidget(editor);
}

void MainWindow::setupFileMenu()
{
	QMenu *fileMenu = new QMenu(tr("&File"), this);
	menuBar()->addMenu(fileMenu);

	fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
			QKeySequence::New);

	fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
			QKeySequence::Open);

	fileMenu->addAction(tr("&Save"), this, SLOT(saveFile()),
			QKeySequence::Save);

	fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
			QKeySequence::Quit);
}

void MainWindow::setupHelpMenu()
{
	QMenu *helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);

	helpMenu->addAction(tr("&Reference"), this, SLOT(about()));
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	// helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}
