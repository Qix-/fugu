#include <QtGui>
#include <Qsci/qsciscintilla.h>

#include "fglexer.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
,mUniverse(NULL)
,mSimulationTimer(NULL)
{
	QApplication::setStyle(new QCleanlooksStyle);

	setWindowTitle(tr("fugu"));

	setupFileMenu();
	setupEditMenu();
	setupSimulationControls();
	setupHelpMenu();

	QWidget *container = new QWidget;
	// container->setStyleSheet("background: #101010;");
	// qlineargradient( x1: 0, y1: 0, x2: 1, y2"	": 0, stop: 0 black, stop: 1 white);");

	QHBoxLayout *layout = new QHBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);

	mEditors = new QTabWidget(container);
	mEditors->setMinimumSize(100,100);
	layout->addWidget(mEditors);
	// mEditors->setTabsClosable(true);
	mEditors->setDocumentMode(false);
	mEditors->setMovable(true);
	mEditors->setUsesScrollButtons(true);

	//mEditors->
	/*
	setStyleSheet(
	      "QTabBar::tab { background: gray; color: white; padding: 10px; } "
	      "QTabBar::tab:selected { background: lightgray; } "
	      "QTabWidget::pane { border: 0; } "
	      "QWidget { background: lightgray; } ");
	      */



	mFGView = new FGView(this);

	/*
	layout()->addWidget(mFGView, BorderLayout::Center);
	layout()->addWidget(mEditors, BorderLayout::West);
	*/

	QSplitter* frame = new QSplitter();
	frame->addWidget(container);
	frame->addWidget(mFGView);
	setCentralWidget(frame);

	QFile stylesheet("fgestyle.css");
	if (stylesheet.open(QFile::ReadOnly | QFile::Text)){
		setStyleSheet(stylesheet.readAll());
	}

	/*
	QDockWidget *dock = new QDockWidget(tr("Editors"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(mEditors);
	addDockWidget(Qt::LeftDockWidgetArea, dock);

	dock = new QDockWidget(tr("FG View"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(mFGView);
	addDockWidget(Qt::RightDockWidgetArea, dock);
	*/

	// setCentralWidget(mEditors);

	mSimulationMode = SM_PAUSED;
	mPreviousMode =SM_PAUSED;
	mTimeMultiplier = 1;
	mTime = 0;

	mSimulationTimer = new QTimer(this);
	connect(mSimulationTimer, SIGNAL(timeout()), this, SLOT(simulateOneStep()));

	newEditor(new QFile("../scripts/ben/aorta.lua"));

	load();
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About fugu"),
			tr("<p><b>fugu</b> is a code-oriented form generation tool.</p>"));
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
				tr("Open File"), "", "fugu script (*.lua)");

	if (!fileName.isEmpty()) {

		// check if its already open...
		// if so, just switch to that file
		for(int i=0;i<mEditors->count();i++){
			if (mFileNames[mEditors->widget(i)]==fileName){
				mEditors->setCurrentIndex(i);
				return;
			}
		}
		// if not, open a new editor
		newEditor(new QFile(fileName));
	}
}

void MainWindow::save(){
	// Get the current opened file..
	QWidget* qw = mEditors->currentWidget();
	if (qw==NULL){
		QMessageBox::critical(this, tr("Application"), tr("No code to save!"));
		return;
	}
	else {
		if (mFileNames.contains(qw)){
			QString fileName = mFileNames[qw];
			if (fileName.isEmpty()){
				saveAs();
			}
			else {
				saveFile(static_cast<QsciScintilla*>(qw),fileName);
			}
		}
	}
}

void MainWindow::saveAs(){
	QWidget* qw = mEditors->currentWidget();
	if (qw==NULL){
		QMessageBox::critical(this, tr("Application"), tr("There is no open file to save!"));
		return;
	}
	else {
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save As..."), "", "fugu script (*.lua)");
		if (fileName.isEmpty())
			return;

		saveFile(static_cast<QsciScintilla*>(qw),fileName);
		// reset filename
		mFileNames[qw] = fileName;
	}
}

void MainWindow::load(){
	std::cout << "Loading universe\n";

	if (mUniverse!=NULL){
		// Clean up the old universe
		mFGView->unsetUniverse();
		delete mUniverse;
		mUniverse = NULL;
		// TODO: make sure old slider values carry over into new state
	}

	try {
		// Create a new universe
		mUniverse = new fg::Universe();
		mUniverse->addScriptDirectory("../scripts/?.lua");
		mUniverse->loadScript("ben/aorta");

		mFGView->setUniverse(mUniverse);
	}
	catch (std::runtime_error& e){
		mSimulationMode = SM_ERROR;
		std::cerr << "ERROR: " << e.what() << "\n";
		if (mUniverse!=NULL){
			delete mUniverse;
			mUniverse = NULL;
		}
	}
}

void MainWindow::unload(){
	if (mUniverse!=NULL){
		mFGView->unsetUniverse();
		delete mUniverse;
		mUniverse = NULL;
	}
}

void MainWindow::togglePlay(bool doplay){
	if (doplay) play();
	else pause();
}

void MainWindow::play(){
	if (mSimulationMode==SM_ERROR) {
		// don't change mode
	}
	else {
		if (mSimulationMode != SM_PLAYING){
			// start the timer
			if (mSimulationTimer==NULL){
				mSimulationTimer = new QTimer(this);
				connect(mSimulationTimer, SIGNAL(timeout()), this, SLOT(simulateOneStep()));
			}
			mSimulationTimer->start(10);
			mSimulationMode = SM_PLAYING;
		}
	}
}

void MainWindow::pause(){
	if (mSimulationMode==SM_ERROR) {
		// don't change mode
	}
	else {
		if (mSimulationTimer!=NULL){
			mSimulationTimer->stop();
		}
		mSimulationMode = SM_PAUSED;
	}

}

void MainWindow::step(){
	if (mSimulationMode==SM_ERROR) {
		// don't change mode
	}
	else {
		if (mSimulationTimer!=NULL){
			mSimulationTimer->stop();
		}
		mSimulationMode = SM_STEPPING;
		simulateOneStep();
		mSimulationMode = SM_PAUSED;
	}
}

void MainWindow::reload(){
	if (mSimulationMode==SM_ERROR) {
		mPreviousMode = SM_PAUSED;
	}
	else {
		mPreviousMode = mSimulationMode;
	}

	mSimulationMode = SM_RELOADING;
	load();

}

void MainWindow::simulateOneStep(){
	if (mUniverse!=NULL){

		try {
			mUniverse->update(0.01);
		}
		catch (std::runtime_error& e){
			std::cerr << "ERROR: " << e.what() << "\n";
			unload();
			mSimulationMode = SM_ERROR;
		}

		mFGView->update();
	}
}

void MainWindow::undo(){
	QWidget* qw = mEditors->currentWidget();
	if (qw!=NULL){
		static_cast<QsciScintilla*>(qw)->undo();
	}
}

void MainWindow::redo(){
	QWidget* qw = mEditors->currentWidget();
	if (qw!=NULL){
		static_cast<QsciScintilla*>(qw)->redo();
	}
}

// Save the the editor's contents to the file fileName
bool MainWindow::saveFile(QsciScintilla* editor, QString fileName){
	// Save the currently active script
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly)) {
		QMessageBox::warning(this, tr("Application"),
							 tr("Cannot write file %1:\n%2.")
							 .arg(fileName)
							 .arg(file.errorString()));
		return false;
	}

	QTextStream out(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	out << editor->text();
	QApplication::restoreOverrideCursor();
	// statusBar()->showMessage(tr("File saved"), 2000);

	// rename tab label ...
	QFileInfo fi(file);
	mEditors->setTabText(mEditors->indexOf(editor),fi.fileName());
	return true;
}


void MainWindow::newEditor(QFile* file)
{
	/*
	QFont font;
	font.setFamily("Courier");
	font.setFixedPitch(true);
	font.setPointSize(10);
	*/

	// text editor
	// see the Qt CodeEditor example for line numbers, and line highlighting
	/*
	QTextEdit* editor = new QTextEdit;
	editor->setFont(font);
	editor->setTabStopWidth(20);
	Highlighter* highlighter = new Highlighter(editor->document());
	*/

	QsciScintilla* editor = new QsciScintilla();
	editor->setLexer(new FGLexer());
	editor->setTabWidth(2);
	editor->setAutoIndent(true);

	editor->setMarginWidth(1,40);
	editor->setMarginLineNumbers(1,true);

	editor->setWrapMode(QsciScintilla::WrapCharacter);

	editor->setCaretForegroundColor(QColor("#959595"));
	editor->setMarginsBackgroundColor(QColor("#272727"));
	editor->setMarginsForegroundColor(QColor("#959595"));

	if (file==NULL){
		// Open a blank editor
		int t = mEditors->addTab(editor, "unnamed");
		mFileNames.insert(editor,QString()); // add empty filename
	}
	else {
		if (file->open(QFile::ReadOnly | QFile::Text)){
			QFileInfo fi(*file);
			// TODO: Store the file pointer with the editor (or somewhere..)
			// Open an editor with the contents of fileName
			int t = mEditors->addTab(editor, fi.fileName());
			// editor->setPlainText(file->readAll());
			editor->setText(file->readAll());
			mFileNames.insert(editor,file->fileName());
		}
		delete file;
	}
	mEditors->setCurrentWidget(editor);

	// setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

void MainWindow::setupFileMenu()
{
	QMenu *fileMenu = new QMenu(tr("&File"), this);
	menuBar()->addMenu(fileMenu);

	fileMenu->addAction(tr("&New"), this, SLOT(newFile()),
			QKeySequence::New);

	fileMenu->addAction(tr("&Open..."), this, SLOT(openFile()),
			QKeySequence::Open);

	fileMenu->addAction(tr("&Save"), this, SLOT(save()),
			QKeySequence::Save);

	fileMenu->addAction(tr("&Save As..."), this, SLOT(saveAs()),
			QKeySequence::SaveAs);

	fileMenu->addAction(tr("E&xit"), qApp, SLOT(quit()),
			QKeySequence::Quit);
}

void MainWindow::setupEditMenu(){
	QMenu *editMenu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(editMenu);

	editMenu->addAction(tr("Undo"), this, SLOT(undo()),
			QKeySequence::Undo);
	editMenu->addAction(tr("Redo"), this, SLOT(redo()),
				QKeySequence::Redo);
}

void MainWindow::setupHelpMenu()
{
	QMenu *helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);

	helpMenu->addAction(tr("&Reference"), this, SLOT(about()));
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	// helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::setupSimulationControls() {
	QMenu* simulationMenu = new QMenu(tr("&Simulation"), this);
	menuBar()->addMenu(simulationMenu);

	QToolBar* simulationToolbar = new QToolBar(tr("&Simulation"), this);
	addToolBar(simulationToolbar);

	QIcon playIcon("../icons/control_play.png");
	playIcon.addPixmap(QPixmap("../icons/control_pause.png"),QIcon::Normal,QIcon::On);
	playIcon.addPixmap(QPixmap("../icons/control_play.png"),QIcon::Normal,QIcon::Off);

	QAction* simulate = new QAction(playIcon, tr("&Play/Pause"), this);
	simulate->setCheckable(true);
	simulate->setStatusTip(tr("Start/Stop the simulation"));
	connect(simulate, SIGNAL(toggled(bool)), this, SLOT(togglePlay(bool)));



	QAction* restart = new QAction(QIcon("../icons/control_start.png"), tr("&Reload"), this);
	restart->setStatusTip(tr("Reload the simulation"));
	connect(restart, SIGNAL(triggered()), this, SLOT(reload()));


	QAction* stepAction = new QAction(QIcon("../icons/control_step.png"), tr("&Step"), this);
	stepAction->setStatusTip(tr("Perform one simulation step"));
	connect(stepAction, SIGNAL(triggered()), this, SLOT(step()));

	simulationToolbar->addAction(restart);
	simulationToolbar->addAction(simulate);
	simulationToolbar->addAction(stepAction);

	simulationMenu->addAction(simulate);
	simulationMenu->addAction(restart);
	simulationMenu->addAction(stepAction);
}
