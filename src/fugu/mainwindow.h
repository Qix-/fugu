#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include <QSet>

#include "fgview.h"

#include "fg/universe.h"

// forward decl
class QTextEdit;
class QMdiArea;
class QTabWidget;
class QFile;
class QString;
class QSlider;
class QsciScintilla;
class ConsoleWidget;
struct lua_State;
namespace luabind {class object;}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	static inline MainWindow* instance(){return sInstance;}

public slots:
	void about();

	void newWindow();

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
	void resetSliders();

	void simulateOneStep();
	void simulateOneStepNoViewUpdate();

	void undo();
	void redo();

	void runScript(QString code);
	void redirectStreams();
	void makeCurrentScriptActive();

	void showLineNumbers(bool);
	void toggleFullScreen(bool);

	void exportSimulation();
	void exportSimulationChooseDir(); // called by exportSimulation only

	void buildFuguKeywordSet();
	void buildReference(); // build the html reference

	void textChanged();
	void paramSliderValueChanged(int val);

	void chooseDrawMode(QString);

	void setBackgroundHorizonColour();
	void setBackgroundSkyColour();

private:

	void openFile(const QString &path = QString());

	// Save the the editor's contents to the file fileName
	bool saveFile(QsciScintilla* editor, QString fileName);

	/// Open a new editor of file (or a blank editor if file==NULL)
	void newEditor(QFile* file=NULL);

	// sets up the slider sheet and add_slider callback
	void setupAddSliderCallback(lua_State*);

	static void lua_add_slider(const luabind::object& o);

	// text editors
	QTabWidget* mEditors;
	QHash<QWidget*,QString> mFileNames; // map: texteditor -> filename
	QWidget* mActiveScript; // the script that is executed when play/restart is pressed...
	QSet<QWidget*> mUnsavedEditors; // the unsaved editors

	const char* mFuguKeywords; // fugu keywords that get syntax highlighting

	// 3d view
	FGView* mFGView;

	// console
	ConsoleWidget* mConsoleWidget;
	QDockWidget* mConsoleDockWidget;

	// controls
	struct BoundVariable {
		std::string var; // lua variable name
		double multiplier; // applied to the slider's value to get the real value
	};

	QDockWidget* mControlWidget;
	QWidget* mControlList;
	QHash<QSlider*, BoundVariable> mBoundVariableMap;

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

	static MainWindow* sInstance;

};

#endif
