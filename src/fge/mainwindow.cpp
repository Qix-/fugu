#include "mainwindow.h"

#include <iostream>

#include <QtGui>
#include <Qsci/qsciscintilla.h>

#include "fglexer.h"
#include "consolewidget.h"
#include "redirect.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
,mUniverse(NULL)
,mSimulationTimer(NULL)
,mSimulationMode(SM_PAUSED)
,mPreviousMode(SM_PAUSED)
,mTimeMultiplier(1)
,mTime(0)
,mActiveScript(NULL)
{
	Ui::MainWindow ui;
	ui.setupUi(this);

	mConsoleWidget = findChild<ConsoleWidget*>("consolewidget");
	mFGView = findChild<FGView*>("fgview");
	mEditors = findChild<QTabWidget*>("editors");

	// XXX: disable redirect for now
	QTimer::singleShot(1000, this, SLOT(redirectStreams()));

	// create action groups..

	QActionGroup* drawModeGroup = new QActionGroup(this);
	drawModeGroup->addAction(ui.actionSetDrawSmooth);
	drawModeGroup->addAction(ui.actionSetDrawFlat);
	drawModeGroup->addAction(ui.actionSetDrawWire);
	drawModeGroup->addAction(ui.actionSetDrawPoints);
	drawModeGroup->addAction(ui.actionSetDrawTextured);
	drawModeGroup->addAction(ui.actionSetDrawPhong);
	ui.actionSetDrawPhong->setChecked(true);

	QActionGroup* subdivModeGroup = new QActionGroup(this);
	subdivModeGroup->addAction(ui.actionSetSubdivs0);
	subdivModeGroup->addAction(ui.actionSetSubdivs1);
	subdivModeGroup->addAction(ui.actionSetSubdivs2);
	subdivModeGroup->addAction(ui.actionSetSubdivs3);
	ui.actionSetSubdivs1->setChecked(true);

	QActionGroup* colourModeGroup = new QActionGroup(this);
	colourModeGroup->addAction(ui.actionSetColourNone);
	colourModeGroup->addAction(ui.actionSetColourVertex);
	ui.actionSetColourVertex->setChecked(true);

	newEditor(new QFile("../scripts/ben/aorta.lua"));
	makeCurrentScriptActive();
	newEditor(new QFile("../scripts/ben/blue.lua"));
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

void MainWindow::open()
{
	openFile();
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

void MainWindow::closeFile(){
	QWidget* qw = mEditors->currentWidget();
	if (qw==NULL){
		QMessageBox::critical(this, tr("Application"), tr("No selected file to close!"));
		return;
	}
	else {
		// TODO: check if we want to save...

		// Close file and remove active
		if (mFileNames.contains(qw)){
			mFileNames.remove(qw);
		}

		if (qw==mActiveScript)
		{
			unload();
			mActiveScript = NULL;
		}

		mEditors->removeTab(mEditors->currentIndex());
		delete qw;
	}
}

void MainWindow::quit(){
	QApplication::exit();
}

void MainWindow::load(){
	if (mActiveScript==NULL){
		QMessageBox::warning(this, tr("No Active Script"), tr("There must be an active script in order to run a simulation."));
		return;
	}
	else {
		std::cout << "Loading universe\n";
		if (mUniverse!=NULL){
			// Clean up the old universe
			mFGView->unsetUniverse();
			delete mUniverse;
			mUniverse = NULL;
			// TODO: make sure old slider values carry over into new state

			// mFGView->repaint();
		}

		try {
			// Create a new universe
			mUniverse = new fg::Universe();

			// add the search paths
			QString filename = mFileNames[mActiveScript];
			QFileInfo info = QFileInfo(filename);

			// QFile file(filename);
			QDir dir = info.dir();
			// dir.makeAbsolute();
			mUniverse->addScriptDirectory((dir.absolutePath() + "/?.lua").toStdString());
			mUniverse->addScriptDirectory("../scripts/?.lua");
			// mUniverse->addScriptDirectory("./?.lua");
			// mUniverse->loadScript("ben/aorta");
			// chop off .lua suffix

			QString filebase = info.completeBaseName();
			if (filebase.endsWith(".lua")){
				filebase.truncate(filebase.length()-4);
			}
			mUniverse->loadScript(filebase.toStdString());

			mFGView->setUniverse(mUniverse);
			// mFGView->repaint();
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

void MainWindow::runScript(QString code){
	// TODO: pause the update timer, run the script, the re-run the update timer
	if (mSimulationTimer->isActive()){
		mSimulationTimer->stop();
		if (mUniverse){
			mUniverse->runScript(code.toStdString());
		}
		mSimulationTimer->start(mSimulationTimer->interval());
	}
	else {
		if (mUniverse){
			mUniverse->runScript(code.toStdString());
		}
	}

}

void MainWindow::redirectStreams(){
	redirectConsoleOutput();
}

void MainWindow::makeCurrentScriptActive(){
	if (mActiveScript){
		int ind = mEditors->indexOf(mActiveScript);
		if (ind!=-1){
			// remove the '[' and ']'
			QFileInfo fi(mFileNames[mActiveScript]);
			mEditors->setTabText(ind,fi.fileName());
		}
	}

	mActiveScript = mEditors->currentWidget();
	if (mActiveScript){
		QFileInfo fi(mFileNames[mActiveScript]);
		// add '[' and ']' to the tab text
		mEditors->setTabText(mEditors->currentIndex(),QString("[") + fi.fileName() + "]");
		reload();
	}
}

void MainWindow::showLineNumbers(bool b){
	for(int i=0;i<mEditors->count();i++){
		QsciScintilla* editor = static_cast<QsciScintilla*>(mEditors->widget(i));
		if (!b){
			editor->setMarginWidth(1,0);
		}
		else {
			editor->setMarginWidth(1,QString("9999"));
		}
	}
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
	if (mActiveScript==editor){
		mEditors->setTabText(mEditors->indexOf(editor),QString("[") + fi.fileName() + "]");
	}
	else {
		mEditors->setTabText(mEditors->indexOf(editor),fi.fileName());
	}
	return true;
}


void MainWindow::newEditor(QFile* file)
{
	QsciScintilla* editor = new QsciScintilla();
	editor->setLexer(new FGLexer());
	editor->setTabWidth(2);
	editor->setAutoIndent(true);

	editor->setMarginWidth(1,QString("9999"));
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
			int t = mEditors->addTab(editor, fi.fileName());
			editor->setText(file->readAll());
			mFileNames.insert(editor,file->fileName());
		}
		delete file;
	}
	mEditors->setCurrentWidget(editor);
}

void MainWindow::setupFileMenu()
{
	/*
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
			*/
}

void MainWindow::setupEditMenu(){
	/*
	QMenu *editMenu = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(editMenu);

	editMenu->addAction(tr("Undo"), this, SLOT(undo()),
			QKeySequence::Undo);
	editMenu->addAction(tr("Redo"), this, SLOT(redo()),
				QKeySequence::Redo);
	*/
}

void MainWindow::setupHelpMenu()
{
	/*
	QMenu *helpMenu = new QMenu(tr("&Help"), this);
	menuBar()->addMenu(helpMenu);

	helpMenu->addAction(tr("&Reference"), this, SLOT(about()));
	helpMenu->addAction(tr("&About"), this, SLOT(about()));
	*/
	// helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}

void MainWindow::setupSimulationControls() {
	/*
	QMenu* simulationMenu = new QMenu(tr("&Simulation"), this);
	menuBar()->addMenu(simulationMenu);

	QToolBar* simulationToolbar = new QToolBar(tr("&Simulation"), this);
	addToolBar(simulationToolbar);
	simulationToolbar->setMovable(false);

	QIcon playIcon("../assets/icons/control_play.png");
	playIcon.addPixmap(QPixmap("../assets/icons/control_pause.png"),QIcon::Normal,QIcon::On);
	playIcon.addPixmap(QPixmap("../assets/icons/control_play.png"),QIcon::Normal,QIcon::Off);

	QAction* simulate = new QAction(playIcon, tr("&Play/Pause"), this);
	simulate->setCheckable(true);
	simulate->setStatusTip(tr("Start/Stop the simulation"));
	connect(simulate, SIGNAL(toggled(bool)), this, SLOT(togglePlay(bool)));

	QAction* restart = new QAction(QIcon("../assets/icons/control_start.png"), tr("&Reload"), this);
	restart->setStatusTip(tr("Reload the simulation"));
	connect(restart, SIGNAL(triggered()), this, SLOT(reload()));


	QAction* stepAction = new QAction(QIcon("../assets/icons/control_step.png"), tr("&Step"), this);
	stepAction->setStatusTip(tr("Perform one simulation step"));
	connect(stepAction, SIGNAL(triggered()), this, SLOT(step()));

	simulationToolbar->addAction(restart);
	simulationToolbar->addAction(simulate);
	simulationToolbar->addAction(stepAction);

	simulationMenu->addAction(simulate);
	simulationMenu->addAction(restart);
	simulationMenu->addAction(stepAction);
	*/
}

void MainWindow::setupViewMenu(){
	QMenu *viewMenu = new QMenu(tr("&View"), this);
	menuBar()->addMenu(viewMenu);

	//QToolBar* viewToolbar = new QToolBar(tr("&View"), this);
	//addToolBar(viewToolbar);



	QAction* action = new QAction(tr("&Origin"),this);
	action->setCheckable(true);
	action->setChecked(true);
	action->setStatusTip(tr("Show the universe origin"));
	connect(action, SIGNAL(toggled(bool)), mFGView, SLOT(toggleOrigin(bool)));
	viewMenu->addAction(action);
	//viewToolbar->addAction(action);

	action = new QAction(tr("&Ground"),this);
	action->setCheckable(true);
	action->setChecked(true);
	action->setStatusTip(tr("Show the ground plane"));
	connect(action, SIGNAL(toggled(bool)), mFGView, SLOT(toggleGround(bool)));
	viewMenu->addAction(action);
	//viewToolbar->addAction(action);

	action = new QAction(tr("&NodeAxes"),this);
	action->setCheckable(true);
	action->setChecked(true);
	action->setStatusTip(tr("Show node axes"));
	connect(action, SIGNAL(toggled(bool)), mFGView, SLOT(toggleShowNodeAxes(bool)));
	viewMenu->addAction(action);
	//viewToolbar->addAction(action);

	action = new QAction(tr("&Lighting"),this);
	action->setCheckable(true);
	action->setChecked(true);
	action->setStatusTip(tr("Enable lighting"));
	connect(action, SIGNAL(toggled(bool)), mFGView, SLOT(toggleLighting(bool)));
	viewMenu->addAction(action);
	//viewToolbar->addAction(action);

	// Rendering mode..
	QAction* setSmooth = new QAction(tr("&Smooth"),this);
	QAction* setFlat = new QAction(tr("&Flat"),this);
	QAction* setWire = new QAction(tr("&Wire"),this);
	QAction* setPoints = new QAction(tr("&Points"),this);
	QAction* setTextured = new QAction(tr("&Textured"),this);
	QAction* setPhong = new QAction(tr("P&hong"),this);
	connect(setSmooth, SIGNAL(triggered()), mFGView, SLOT(setDrawSmooth()));
	connect(setFlat, SIGNAL(triggered()), mFGView, SLOT(setDrawFlat()));
	connect(setWire, SIGNAL(triggered()), mFGView, SLOT(setDrawWire()));
	connect(setPoints, SIGNAL(triggered()), mFGView, SLOT(setDrawPoints()));
	connect(setTextured, SIGNAL(triggered()), mFGView, SLOT(setDrawTextured()));
	connect(setPhong, SIGNAL(triggered()), mFGView, SLOT(setDrawPhong()));

	setSmooth->setCheckable(true);
	setFlat->setCheckable(true);
	setWire->setCheckable(true);
	setPoints->setCheckable(true);
	setTextured->setCheckable(true);
	setPhong->setCheckable(true);

	QActionGroup* drawModeGroup = new QActionGroup(this);
	drawModeGroup->addAction(setSmooth);
	drawModeGroup->addAction(setFlat);
	drawModeGroup->addAction(setWire);
	drawModeGroup->addAction(setPoints);
	drawModeGroup->addAction(setTextured);
	drawModeGroup->addAction(setPhong);
	setPhong->setChecked(true);

	viewMenu->addSeparator()->setText(tr("Draw Mode"));
	viewMenu->addAction(setSmooth);
	viewMenu->addAction(setFlat);
	viewMenu->addAction(setWire);
	viewMenu->addAction(setPoints);
	viewMenu->addAction(setTextured);
	viewMenu->addAction(setPhong);
}

void MainWindow::setupConsoleWidget(){
	mConsoleWidget = new ConsoleWidget();
	connect(mConsoleWidget, SIGNAL(emitCommand(QString)), this, SLOT(runScript(QString)));
}
