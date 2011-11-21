#include "mainwindow.h"

#include <iostream>

#include <QtGui>
#include <QProcess>
#include <QColorDialog>
#include <QSettings>

#include <Qsci/qsciscintilla.h>

#include "luabind/luabind.hpp"
#include "luabind/object.hpp"

#include "fglexer.h"
#include "consolewidget.h"
#include "redirect.h"
#include "exporter.h"

#include "ui_mainwindow.h"
#include "ui_exportdialog.h"

#include "html_template.h"

MainWindow* MainWindow::sInstance = NULL;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
,mUniverse(NULL)
,mSimulationTimer(NULL)
,mSimulationMode(SM_PAUSED)
,mPreviousMode(SM_PAUSED)
,mTimeMultiplier(1)
,mTime(0)
,mActiveScript(NULL)
,mFuguKeywords(NULL)
,mHasSeenControlsBefore(false)
,mFGLexer(NULL)
,mStdOutRedirector(NULL)
{
	sInstance = this;

	Ui::MainWindow ui;
	ui.setupUi(this);

	// set the icon
	QIcon icon;
	icon.addFile(":/res/fg_logo_16.png",QSize(16,16));
	icon.addFile(":/res/fg_logo_32.png",QSize(32,32));
	icon.addFile(":/res/fg_logo_64.png",QSize(64,64));
	setWindowIcon(icon);

	// create console widget
	mConsoleDockWidget = new QDockWidget(tr("Console"), this);
	mConsoleWidget = new ConsoleWidget(mConsoleDockWidget);
	mConsoleDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea); // Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	mConsoleDockWidget->setWidget(mConsoleWidget);
	addDockWidget(Qt::BottomDockWidgetArea, mConsoleDockWidget);
	mConsoleDockWidget->resize(mConsoleDockWidget->minimumSize());
	mConsoleWidget->resize(mConsoleWidget->minimumSize());
	connect(mConsoleWidget, SIGNAL(emitCommand(QString)), this, SLOT(runScript(QString)));

	// mConsoleDockWidget->setFloating(true);
	// mConsoleDockWidget->show();
	findChild<QMenu*>("menuView")->addAction(mConsoleDockWidget->toggleViewAction());

	// mConsoleWidget = findChild<ConsoleWidget*>("consolewidget");

	// grab references to child widgets
	mFGView = findChild<FGView*>("fgview");
	mEditors = findChild<QTabWidget*>("editors");

	QTimer::singleShot(1, this, SLOT(redirectStreams()));

	// create the slider dialog
	mControlWidget = new QDockWidget(tr("Control"),this);
		mControlWidget->setAllowedAreas(Qt::NoDockWidgetArea); // Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

		QWidget* controlContainer = new QWidget();
		QVBoxLayout* ccvb = new QVBoxLayout();

			QPushButton* ccreset = new QPushButton("Reset");
			connect(ccreset, SIGNAL(clicked()), this, SLOT(resetSlidersToDefaults()));
			ccvb->addWidget(ccreset);

			mControlList = new QWidget();
			mControlList->setLayout(new QGridLayout()); // (new QVBoxLayout());

			ccvb->addWidget(mControlList);

		controlContainer->setLayout(ccvb);

		mControlWidget->setWidget(controlContainer);
	addDockWidget(Qt::RightDockWidgetArea, mControlWidget);
	mControlWidget->setFloating(true);
	mControlWidget->hide();
	findChild<QMenu*>("menuView")->addAction(mControlWidget->toggleViewAction());


	// create action groups..

	QActionGroup* drawModeGroup = new QActionGroup(this);
	drawModeGroup->addAction(ui.actionSetDrawSmooth);
	drawModeGroup->addAction(ui.actionSetDrawFlat);
	drawModeGroup->addAction(ui.actionSetDrawWire);
	drawModeGroup->addAction(ui.actionSetDrawPoints);
	drawModeGroup->addAction(ui.actionSetDrawTextured);
	drawModeGroup->addAction(ui.actionSetDrawPhong);
	ui.actionSetDrawFlat->setChecked(true);

	QActionGroup* subdivModeGroup = new QActionGroup(this);
	subdivModeGroup->addAction(ui.actionSetSubdivs0);
	subdivModeGroup->addAction(ui.actionSetSubdivs1);
	subdivModeGroup->addAction(ui.actionSetSubdivs2);
	subdivModeGroup->addAction(ui.actionSetSubdivs3);
	ui.actionSetSubdivs0->setChecked(true);

	QActionGroup* colourModeGroup = new QActionGroup(this);
	colourModeGroup->addAction(ui.actionSetColourNone);
	colourModeGroup->addAction(ui.actionSetColourVertex);
	ui.actionSetColourVertex->setChecked(true);

	// load properties
	ui.actionToggleOrigin->setChecked(mFGView->origin());
	ui.actionToggleGround->setChecked(mFGView->ground());
	ui.actionToggleShowNodeOrigins->setChecked(mFGView->showNodeAxes());
	ui.actionToggleLighting->setChecked(mFGView->lighting());
	ui.actionToggleShowOverWire->setChecked(mFGView->showOverWire());

	QSettings settings("MonashUniversity", "Fugu");
	findChild<QAction*>("actionShowLineNumbers")->setChecked(settings.value("editor/showLineNumbers",false).toBool());

	// add some actions to mainwindow too (so they don't disable when the menu bar is hidden)
	this->addAction(ui.actionToolBar);
	this->addAction(ui.actionMenuBar);
	this->addAction(ui.actionFull_Screen);
	this->addAction(ui.actionReload);

	// build the set of fugu keywords
	buildFuguKeywordSet();

	// build the examples menu
	buildExamplesMenu();

	// load a script
	newEditor(new QFile("../scripts/ex/basic/superformula_ex.lua"));
}

MainWindow::~MainWindow(){
	QSettings settings("MonashUniversity", "Fugu");
	settings.setValue("window/maximised", isMaximized());
	settings.setValue("editor/showLineNumbers", findChild<QAction*>("actionShowLineNumbers")->isChecked());

	mStdOutRedirector->quit();
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About fugu"),
			tr("<p><b>fugu</b> is a code-oriented form generation tool.</p>"));
}

void MainWindow::newWindow(){
	// ...
	QProcess p;
	p.startDetached("fugu");

}

/*
void MainWindow::openProject(){
	// TODO: Close down and save old project

	// Open new project
	QString dir = QFileDialog::getExistingDirectory(this, tr("Open fugu project"),
	                                                 "../scripts/examples",
	                                                 QFileDialog::ShowDirsOnly
	                                                 | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
/ *
		// TODO: use the .fugu project file to store the list of files,
		// but for now, open all the .lua scripts in this directory
		QDir projDir = dir;
		if (not projDir.exists()){
			std::cerr << "Project directory doesn't exist!";
			return;
		}

		QStringList filters;
		filters << "*.lua";
		projDir.setNameFilters(filters);

		foreach(QString file, projDir.entryList()){
			std::cout << "file: " << file;
			newEditor(new QFile(file));
		}
		*/

		/*
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
		* /
	}
}
*/

void MainWindow::newFile()
{
	// editor->clear();
	newEditor();
}

void MainWindow::open()
{
	openFile();
}

void MainWindow::openExample(){
	QAction* qa = static_cast<QAction*>(QObject::sender());
	QString path = qa->data().toString();
	openFile(path);
}

void MainWindow::openFile(const QString &path)
{
	QString fileName = path;
	if (fileName.isNull()){
		QStringList fileNames = QFileDialog::getOpenFileNames(this,
				tr("Open File"), "../scripts", "fugu scripts (*.lua)");
		foreach(QString file, fileNames){
			// check if its already open...
			// if so, just switch to that file
			int i = 0;
			for(;i<mEditors->count();i++){
				if (mFileNames[mEditors->widget(i)]==file){
					mEditors->setCurrentIndex(i);
					break;
				}
			}
			if (i==mEditors->count()){
				// if not, open a new editor
				newEditor(new QFile(file));
			}
		}
	}
	else {
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

			if (mUnsavedEditors.contains(qw)){
				mUnsavedEditors.remove(qw);
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

		if (mUnsavedEditors.contains(qw)){
			mUnsavedEditors.remove(qw);
		}
	}


}

void MainWindow::closeFile(){
	QWidget* qw = mEditors->currentWidget();
	if (qw==NULL){
		QMessageBox::critical(this, tr("Close"), tr("No selected file to close!"));
		return;
	}
	else {
		// TODO: check if we want to save...
		if (mUnsavedEditors.contains(qw)){
			QMessageBox::StandardButton result = QMessageBox::question(this, tr("Close"), tr("Script hasn't been saved, are you sure you want to close it?"), QMessageBox::Yes | QMessageBox::No );
			if (result==QMessageBox::No) return;
		}

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
		if (mUnsavedEditors.contains(qw)){
			mUnsavedEditors.remove(qw);
		}

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

		if (mUnsavedEditors.contains(mActiveScript)){
			QMessageBox::StandardButton result = QMessageBox::question(this,
					tr("Loading unsaved script"),
					tr("The active script is not saved, save it now? (If not then the last saved version will be reloaded instead.)"),
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
			);

			if (result==QMessageBox::Cancel){
				mSimulationMode = mPreviousMode;
				return;
			}
			else if (result==QMessageBox::Yes){
				save();
			}
			else {} // continue on...
		}

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

			// load the add_slider callbacks
			setupAddSliderCallback(mUniverse->getLuaState());

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
		if (mSimulationTimer) mSimulationTimer->stop();
		findChild<QAction*>("actionPlay")->setChecked(false);
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
		findChild<QAction*>("actionPlay")->setChecked(false);
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
		findChild<QAction*>("actionPlay")->setChecked(false);
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

void MainWindow::resetSliders(){

	std::cout << "Resetting sliders\n";

	// if (mControlList) delete 	mControlList;
	// mControlList = new QWidget(mControlWidget);
	// mControlList->layout()->deleteAllItems(); // Qt3

	QLayoutItem *child;
	while ((child = mControlList->layout()->takeAt(0)) != 0) {
		delete child->widget();
	  delete child;
	}

	/*
	QGridLayout* qgd = static_cast<QGridLayout*>(mControlList->layout());
	int rows = qgd->rowCount();
	for(int row=0;row<rows;row++){
		qgb->
	}
	delete mControlList->layout();
	mControlList->setLayout(new QGridLayout()); //(new QVBoxLayout());
	mControlList->layout()->update();
	*/
	// mControlWidget->setWidget(mControlList);

	mBoundVariableMap.clear();
	mSliderToLabelMap.clear();
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

void MainWindow::simulateOneStepNoViewUpdate(){
	if (mUniverse!=NULL){
		try {
			mUniverse->update(0.01);
		}
		catch (std::runtime_error& e){
			std::cerr << "ERROR: " << e.what() << "\n";
			unload();
			mSimulationMode = SM_ERROR;
		}
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
	// std::cout << "Command: \"" << code.toStdString() << "\"\n";

	if (mUniverse and mSimulationTimer!=NULL and mSimulationTimer->isActive()){
		mSimulationTimer->stop();
		mUniverse->runScript(code.toStdString());
		mSimulationTimer->start(mSimulationTimer->interval());
	}
	else if (mSimulationMode!=SM_ERROR and mUniverse!=NULL){
		mUniverse->runScript(code.toStdString());
	}
	else {
		std::cerr << "Script must be active and universe successfully loaded to run a command.\n";
	}
}

void MainWindow::redirectStreams(){
	mStdOutRedirector = redirectConsoleOutput(this);
	if (mStdOutRedirector){
		(void)connect( mStdOutRedirector, SIGNAL( caughtString( QString ) ),
				mConsoleWidget, SLOT( print( QString ) ));
		mStdOutRedirector->start();
	}
}

void MainWindow::makeCurrentScriptActive(){
	// Do nothing if its the same
	if (mActiveScript==mEditors->currentWidget()){
		return;
	}

	// Remove the icon of current active script
	if (mActiveScript){
		int ind = mEditors->indexOf(mActiveScript);
		if (ind!=-1){
			mEditors->setTabIcon(ind,QIcon());
		}
	}

	// Set the active script and its icon
	mActiveScript = mEditors->currentWidget();
	if (mActiveScript){
		mEditors->setTabIcon(mEditors->currentIndex(), QIcon(":/res/control_main_small.png"));
		resetSliders();
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

void MainWindow::toggleFullScreen(bool b){
	if (b){
		showFullScreen();
	}
	else {
		showNormal();
	}

}

void MainWindow::tabChanged(int t){
	// save settings of last tab, and load into new tab

	/*
	QSettings settings("MonashUniversity", "Fugu");
	fgLexer->readSettings(settings,"editor");
	*/

}

void MainWindow::exportSimulation(){
	if (mUniverse==NULL){
		QMessageBox::critical(this, tr("Export"), tr("There is no simulation to export!"));
	}
	else {
		// stop the simulation
		// pause();
		findChild<QAction*>("actionPlay")->setChecked(false);

		mExportDialog = new QDialog(this);
		Ui::ExportDialog ui;
		ui.setupUi(mExportDialog);

		QLineEdit* qe = mExportDialog->findChild<QLineEdit*>("dirLineEdit");
		qe->setText(QDir("export").absolutePath());

		QPushButton* chooseDir = mExportDialog->findChild<QPushButton*>("chooseDirectoryPushButton");
		connect(chooseDir, SIGNAL(pressed()), this, SLOT(exportSimulationChooseDir()));

		QLineEdit* numFramesLE = mExportDialog->findChild<QLineEdit*>("numFramesLineEdit");

		if (mExportDialog->exec()){

			bool convert = false;
			int numFrames = numFramesLE->text().toInt(&convert);
			if (not convert or numFrames<=0){
				QMessageBox::critical(this, tr("Export"), tr("Number of frames is invalid or less than 1!"));
				return;
			}
			// else ...

			std::cout << "Saving to: " << QDir(qe->text()).absolutePath().toStdString() << "\n";

			QProgressDialog progress("Exporting...", "Abort", 0, numFrames, mExportDialog);
			progress.setWindowModality(Qt::WindowModal);
			progress.setAutoClose(true);
			progress.setMinimumDuration(1000); // 1 second..
			Exporter e = Exporter::ExportFrameToObj(mUniverse).dir(QDir(qe->text()));

			for (int i = 0; i < numFrames; i++) {
				progress.setValue(i);

				if (progress.wasCanceled())
					break;
				if (not e.run(i,numFrames)){
					QMessageBox::critical(this, tr("Export"), e.error());
					break;
				}
				simulateOneStepNoViewUpdate();
				if (mUniverse==NULL){
					QMessageBox::critical(this, tr("Export"), tr("The universe imploded, sorry."));
					break;
				}

			}
			progress.setValue(numFrames);

		}
	}
}

void MainWindow::exportSimulationChooseDir(){
	// QMessageBox::information(mExportDialog, tr("Choose Dir"), tr("You pressed choose dir dawg!"));
	QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Export Directory"),
			mExportDialog->findChild<QLineEdit*>("dirLineEdit")->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (not dir.isEmpty()){
		mExportDialog->findChild<QLineEdit*>("dirLineEdit")->setText(QDir(dir).absolutePath());
	}
}

// helpers for the reference building routine
typedef pair<std::string,std::string> string2;
bool sortByName (const string2& a, const string2& b) { return (a.first < b.first); }


// html replace snippet from
// http://www.daniweb.com/software-development/cpp/threads/66386
struct HTMLReplace {
	std::string match, replace;
} codes[] = {
		{"&","&amp;"},
		{"<","&lt;"},
		{">","&gt;"},
};
std::string HTMLEncode(const std::string& s){
	std::string rs = s;
	for(size_t i = 0; i<3 /* num codes*/; i++){
		const std::string& match = codes[i].match;
		const std::string& repl = codes[i].replace;
		std::string::size_type start = rs.find_first_of(match);
		while(start!=std::string::npos){
			rs.replace(start,match.size(),repl);
			start = rs.find_first_of(match,start+repl.size());
		}
	}
	return rs;
}

// Encode a function name to a proper id
struct IdReplace {
	std::string match, replace;
} idcodes[] = {
		{".","dot"},
};
std::string IDEncode(const std::string& s){
	std::string rs = s;
	for(size_t i = 0; i<1 /* num codes*/; i++){
		const std::string& match = idcodes[i].match;
		const std::string& repl = idcodes[i].replace;
		std::string::size_type start = rs.find_first_of(match);
		while(start!=std::string::npos){
			rs.replace(start,match.size(),repl);
			start = rs.find_first_of(match,start+repl.size());
		}
	}
	return rs;
}


void MainWindow::buildFuguKeywordSet(){
	// mFuguKeywords
	if (mFuguKeywords!=NULL){
		std::cerr << "The fugu keyword set is trying to be built more than once!";
		return;
	}

	// if universe doesn't exist, make one temporarily...
	bool tmpuni = false;
	if (mUniverse==NULL){
		tmpuni = true;
		try {
			mUniverse = new fg::Universe();
		}
		catch (std::runtime_error& e){
			mSimulationMode = SM_ERROR;
			std::cerr << "ERROR: " << e.what() << "\n";
			if (mUniverse!=NULL){
				delete mUniverse;
				mUniverse = NULL;
				return;
			}
		}
	}

	typedef tuple<std::string,std::string,std::string> string3;
	std::list<string3> commandList = mUniverse->commandListByCategory();

	std::ostringstream oss;
	foreach(const string3& s3, commandList){
		std::string categoryName = s3.get<0>();
		std::string functionName = s3.get<1>();
		std::string docString = s3.get<2>();
		oss << functionName << " ";
	}

	mFuguKeywords = strdup(oss.str().c_str());

	if (tmpuni){
		delete mUniverse;
		mUniverse = NULL;
	}

}

void MainWindow::buildReference() // build the html reference
{
	// if universe doesn't exist, make one temporarily...
	bool tmpuni = false;
	if (mUniverse==NULL){
		tmpuni = true;
		try {
			mUniverse = new fg::Universe();
		}
		catch (std::runtime_error& e){
			mSimulationMode = SM_ERROR;
			std::cerr << "ERROR: " << e.what() << "\n";
			if (mUniverse!=NULL){
				delete mUniverse;
				mUniverse = NULL;
				return;
			}
		}
	}

	QFile file("../doc/ref/reference.html");
	if (file.open(QIODevice::WriteOnly)){
		QTextStream out(&file);

		tmpl::html_template templ("../doc/ref/reference.tmpl");
		templ("DATE") = QDate::currentDate().toString("dd/MM/yyyy").toStdString();

		typedef tuple<std::string,std::string,std::string> string3;
		std::list<string3> commandList = mUniverse->commandListByCategory();
		tmpl::loop_t category[3];
		std::string currentCategoryName;
		tmpl::row_t categoryRow;
		tmpl::loop_t functionLoop;

		// divide the commands up equally into the three columns
		int numCPerCol = (commandList.size()+2)/3;
		int currentCol = 0;
		int currentCCount = 0;

		std::vector<string2> currentCategoryFunctions;

		foreach(const string3& s3, commandList){
			std::string categoryName = s3.get<0>();
			std::string functionName = s3.get<1>();
			std::string docString = s3.get<2>();

			if (categoryName!=currentCategoryName){
				// end old category
				if (not currentCategoryName.empty()){
					// sort functions
					std::sort(currentCategoryFunctions.begin(),currentCategoryFunctions.end(),sortByName);
					// currentCategoryFunctions.sort

					foreach(string2 p, currentCategoryFunctions){
						tmpl::row_t fr;
						fr("FUNCTION_NAME") = p.first;
						fr("FUNCTION_ID") = IDEncode(p.first);

						/* encode the document in html */
						fr("FUNCTION_DOC") = HTMLEncode(p.second);
						fr("HAS_DOC") = p.second.empty()?"has_no_doc":"has_doc";
						functionLoop += fr;
					}

					categoryRow("FUNCTIONS") = functionLoop;
					category[currentCol] += categoryRow;
				}

				// start a new category
				currentCategoryFunctions.clear();
				currentCategoryName = categoryName;
				categoryRow = tmpl::row_t();
				categoryRow("CATEGORY") = categoryName;
				functionLoop = tmpl::loop_t();

				if (currentCCount > numCPerCol){
					currentCCount = 0;
					currentCol++;
				}
			}

			// add function to category
			currentCategoryFunctions.push_back(make_pair(functionName,docString));
			currentCCount++;
		}

		//
		// sort functions
		std::sort(currentCategoryFunctions.begin(),currentCategoryFunctions.end(),sortByName);
		// currentCategoryFunctions.sort

		foreach(string2 p, currentCategoryFunctions){
			tmpl::row_t fr;
			fr("FUNCTION_NAME") = p.first;
			fr("FUNCTION_ID") = IDEncode(p.first);
			fr("FUNCTION_DOC") = HTMLEncode(p.second);
			fr("HAS_DOC") = p.second.empty()?"has_no_doc":"has_doc";
			functionLoop += fr;
		}

		categoryRow("FUNCTIONS") = functionLoop;
		category[currentCol] += categoryRow;

		// add the final bunch of functions
		//categoryRow("FUNCTIONS") = functionLoop;
		//category[currentCol] += categoryRow;
		// add the categories
		templ("CATEGORIES1") = category[0];
		templ("CATEGORIES2") = category[1];
		templ("CATEGORIES3") = category[2];

		std::ostringstream str;
		str << templ;
		out << QString::fromStdString(str.str());
	}
	else {
		std::cerr << "Whoops, can't open reference.html.";
	}

	if (tmpuni){
		delete mUniverse;
		mUniverse = NULL;
	}
}

void MainWindow::buildExamplesMenu(){
	// go through and build the examples menu
	QMenu* menuExamples = findChild<QMenu*>("menuFile")->findChild<QMenu*>("menuExamples");
	menuExamples->clear();

	QDir examplesDir = QDir("../scripts/ex");

	foreach(QString subdir, examplesDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)){
		QMenu* subMenu = new QMenu(subdir,menuExamples);
		QDir examplesSubDir = examplesDir.filePath(subdir);
		foreach(QString file, examplesSubDir.entryList(QStringList("*.lua"))){
			QString path = examplesSubDir.filePath(file);
			QAction* qa = new QAction(subMenu);
			QString name = file;
			name.chop(4); // remove .lua
			qa->setText(name);
			qa->setData(path);
			subMenu->addAction(qa);
			connect(qa,SIGNAL(triggered()),this,SLOT(openExample()));
		}
		menuExamples->addMenu(subMenu);
	}
}

void MainWindow::textChanged(){
	// the text of the current editor has changed...
	// so mark it as unsaved...
	QWidget* qw = mEditors->currentWidget();
	if (qw==NULL){
		// not possible, ignore..
	}
	else {
		if (not mUnsavedEditors.contains(qw)){
			// mark it as unsaved...
			mUnsavedEditors.insert(qw);

			QString filename = mFileNames[qw];
			QFileInfo info = QFileInfo(filename);

			/*
			if (qw==mActiveScript){
				mEditors->setTabText(mEditors->currentIndex(), QString("[") + info.fileName() + "]*");
			}
			else {*/
				mEditors->setTabText(mEditors->currentIndex(), info.fileName() + "*");
			// }
		}
	}
}

void MainWindow::paramSliderValueChanged(int val){
	QSlider* qs = static_cast<QSlider*>(QObject::sender());
	BoundVariable bv = mBoundVariableMap[qs];

	int index = mEditors->indexOf(mActiveScript);
	QString str = mEditors->tabText(index);
	str = str.mid(0,str.length()-4);
	str += QString(".") + QString::fromStdString(bv.var) + QString("=") + QString::number(val/bv.multiplier);
	mSliderToLabelMap[qs]->setNum(val/bv.multiplier);

	runScript(str);
}

void MainWindow::resetSlidersToDefaults(){
	typedef QHash<QSlider*, BoundVariable> SliderHash;
	SliderHash::iterator it = mBoundVariableMap.begin();
	for(;it!=mBoundVariableMap.end();it++){
		const BoundVariable& bv = it.value();

		QSlider* qs = it.key();
		qs->setValue(bv.def*bv.multiplier);
		mSliderToLabelMap[qs]->setNum(bv.def);

		int index = mEditors->indexOf(mActiveScript);
		QString str = mEditors->tabText(index);
		str = str.mid(0,str.length()-4);
		str += QString(".") + QString::fromStdString(bv.var) + QString("=") + QString::number(bv.def);
		runScript(str);
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
	QsciScintilla* editor = new QsciScintilla();

	if (mFGLexer==NULL){
		mFGLexer = new FGLexer(this);
		mFGLexer->registerFuguKeywords(mFuguKeywords);
	}

	//FGLexer* fgLexer = new FGLexer(editor);

	//QSettings settings("MonashUniversity", "Fugu");
	//fgLexer->readSettings(settings,"editor");

	// fgLexer->registerFuguKeywords(mFuguKeywords);
	// editor->setLexer(fgLexer);
	editor->setLexer(mFGLexer);
	editor->setTabWidth(2);
	editor->setAutoIndent(true);

	editor->setMarginLineNumbers(1,true);
	bool b = findChild<QAction*>("actionShowLineNumbers")->isChecked();
	if (!b){
		editor->setMarginWidth(1,0);
	}
	else {
		editor->setMarginWidth(1,QString("9999"));
	}
	editor->setMarginWidth(2,8); // a little margin

	editor->setWrapMode(QsciScintilla::WrapCharacter);

	editor->setCaretForegroundColor(QColor("#959595"));
	editor->setMarginsBackgroundColor(QColor("#272727"));
	editor->setMarginsForegroundColor(QColor("#959595"));

	if (file==NULL){
		// Open a blank editor
		int t = mEditors->addTab(editor, "unnamed.lua");
		mFileNames.insert(editor,QString("unnamed.lua")); // add empty filename
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
	connect(editor,SIGNAL(textChanged()),this,SLOT(textChanged()));

	if (!mActiveScript) makeCurrentScriptActive();
}

void MainWindow::setupAddSliderCallback(lua_State* L){
	// setup function
	luabind::module(L)[
	   luabind::def("add_slider", &MainWindow::lua_add_slider)
	];
}

void MainWindow::lua_add_slider(const luabind::object& o){
	MainWindow* win = MainWindow::instance();

	if (!win->mHasSeenControlsBefore){
		win->mHasSeenControlsBefore = true;
		win->mControlWidget->show();
	}

	// parse the contents
	if (o["var"] and o["low"] and o["high"] and o["value"]){
		std::string var = luabind::object_cast<std::string>(o["var"]);
		double value = luabind::object_cast<double>(o["value"]);
		double low = luabind::object_cast<double>(o["low"]);
		double high = luabind::object_cast<double>(o["high"]);

		typedef QHash<QSlider*, BoundVariable> SliderHash;

		SliderHash::iterator it = win->mBoundVariableMap.begin();
		for(;it!=win->mBoundVariableMap.end();it++){
			const BoundVariable& bv = it.value();
			if (bv.var == var){
				if (bv.low == low and bv.high == high and bv.def==value){
					QSlider* qs = it.key();
					qs->setValue(qs->value());
					int index = win->mEditors->indexOf(win->mActiveScript);
					QString str = win->mEditors->tabText(index);
					str = str.mid(0,str.length()-4);
					str += QString(".") + QString::fromStdString(bv.var) + QString("=") + QString::number(qs->value()/bv.multiplier);
					win->mSliderToLabelMap[qs]->setNum(qs->value()/bv.multiplier);
					win->runScript(str);
					return;
				}
				else {
					break;
				}
			}
		}

		// if the iterator hasn't reached the end
		// then rebuild the slider ..
		bool rebuildSlider = false;
		QSlider* qs = NULL;
		if (it!=win->mBoundVariableMap.end()){
			rebuildSlider = true;
			qs = it.key();
		}

		/*
		// only add it if it isn't in the list already...
		foreach(const BoundVariable& v, mBoundVariableMap.values()){
			if (v.var == var){
				// TODO: set the variable to its old value

				return;
			}
		}
		*/
		// else add it


		// std::cout << "Binding var \"" << var << "\" with {" << value << "," << low << "," << high <<"}\n";

		// as a qslider operates on integers, figure out the multiplier to get from int_val->double_val
		double range = high - low;
		if (range < 0){
			std::cerr << "add_slider parameter error (high < low)";
			return;
		}

		double multiplier = 1;
		// for low,high = (-100,100) no need for a multiplier
		// for low,high = (0,1) the multiplier should be 100
		if (range < 1){
			multiplier = 100;
		}
		else if (range < 10){
			multiplier = 10;
		}
		else if (range < 100){
			multiplier = 100;
		}
		else multiplier = 1000;

		if (!qs){
			qs = new QSlider(Qt::Horizontal);
		}

		qs->setMinimum(low*multiplier);
		qs->setMaximum(high*multiplier);

		if (!rebuildSlider){
			QLabel* ql = new QLabel(QString::fromStdString(var));
			QLabel* qv = new QLabel("0");
			qv->setMinimumWidth(qv->fontMetrics().tightBoundingRect("000000").width());

			QGridLayout* qg = static_cast<QGridLayout*>(win->mControlList->layout());
			int rows = qg->rowCount();
			qg->addWidget(ql,rows,0);
			qg->addWidget(qs,rows,1);
			qg->addWidget(qv,rows,2);

			connect(qs,SIGNAL(valueChanged(int)),win,SLOT(paramSliderValueChanged(int)));


			win->mSliderToLabelMap.insert(qs,qv);
		}

		BoundVariable bv;
		bv.var = var;
		bv.multiplier = multiplier;
		bv.low = low;
		bv.high = high;
		bv.def = value;
		win->mBoundVariableMap.insert(qs,bv);


		qs->setValue(value*multiplier);
	}
	else {
		std::cerr << "add_slider needs the following params: \"var\", \"value\", \"low\", \"high\"\n";
	}
}

void MainWindow::chooseDrawMode(QString mode){
	if (mode=="points"){
		mFGView->setDrawPoints();
	}
	else if (mode=="wireframe"){
		mFGView->setDrawWire();
	}
	else if (mode=="flat shaded"){
		mFGView->setDrawFlat();
	}
	else if (mode=="phong shaded"){
		mFGView->setDrawPhong();
	}
	else if (mode=="textured"){
		mFGView->setDrawTextured();
	}
}

void MainWindow::setBackgroundHorizonColour(){
	QColor col = QColorDialog::getColor(mFGView->getBackgroundHorizonColour(),this, "Choose Background Horizon Colour");
	if (col.isValid()){
		mFGView->setBackgroundHorizonColour(col);
	}
}

void MainWindow::setBackgroundSkyColour(){
	QColor col = QColorDialog::getColor(mFGView->getBackgroundSkyColour(),this, "Choose Background Sky Colour");
	if (col.isValid()){
		mFGView->setBackgroundSkyColour(col);
	}
}
