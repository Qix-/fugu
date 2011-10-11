#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

// #include "highlighter.h"
#include "fgview.h"
#include "fg/universe.h"

// forward decl
class QTextEdit;
class QMdiArea;
class QTabWidget;
class QFile;
class QString;
class QsciScintilla;
class ConsoleWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

public slots:
	void about();
	void newFile();
	void open();
	void save();
	void saveAs();
	void closeFile();
	void quit();


	void load(); // load the current script
	void unload(); // unload the current universe

	void togglePlay(bool);
	void play();
	void pause();
	void step();
	void reload();

	void simulateOneStep();

	void undo();
	void redo();

	void runScript(QString code);
	void redirectStreams();
	void makeCurrentScriptActive();

	void showLineNumbers(bool);

	void exportSimulation();
	void exportSimulationChooseDir(); // called by exportSimulation only

private:

	void openFile(const QString &path = QString());

	// Save the the editor's contents to the file fileName
	bool saveFile(QsciScintilla* editor, QString fileName);

	/// Open a new editor of file (or a blank editor if file==NULL)
	void newEditor(QFile* file=NULL);

	// text editors
	QTabWidget* mEditors;
	QHash<QWidget*,QString> mFileNames; // map: texteditor -> filename
	QWidget* mActiveScript; // the script that is executed when play/restart is pressed...

	// 3d view
	FGView* mFGView;

	// console
	ConsoleWidget* mConsoleWidget;

	// export dialog
	QDialog* mExportDialog;

	// Current simulation state ...
	enum SimulationMode {SM_PLAYING, SM_PAUSED, SM_STEPPING, SM_RELOADING, SM_ERROR};

	fg::Universe* mUniverse;
	SimulationMode mSimulationMode;
	SimulationMode mPreviousMode;
	double mTimeMultiplier;
	double mTime;

	QTimer* mSimulationTimer;
};

#endif
