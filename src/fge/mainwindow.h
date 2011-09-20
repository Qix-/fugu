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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

public slots:
	void about();
	void newFile();
	void openFile(const QString &path = QString());
	void save();
	void saveAs();

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
private:

	// Save the the editor's contents to the file fileName
	bool saveFile(QsciScintilla* editor, QString fileName);

	/// Open a new editor of file (or a blank editor if file==NULL)
	void newEditor(QFile* file=NULL);

	void setupFileMenu();
	void setupEditMenu();
	void setupHelpMenu();
	void setupViewMenu();
	void setupSimulationControls();

	// text editors
	QTabWidget* mEditors;
	QHash<QWidget*,QString> mFileNames; // map: texteditor -> filename

	// 3d view
	FGView* mFGView;

	// console
	// QTextEdit* mConsole;

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
