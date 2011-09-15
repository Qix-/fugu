#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "highlighter.h"
#include "fgview.h"

// forward decl
class QTextEdit;
class QMdiArea;
class QTabWidget;
class QFile;
class QString;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

public slots:
void about();
void newFile();
void openFile(const QString &path = QString());
void saveFile();

private:
/// Open a new editor of file (or a blank editor if file==NULL)
void newEditor(QFile* file=NULL);

void setupFileMenu();
void setupHelpMenu();

// text editors
QTabWidget* mEditors;

// console
// QTextEdit* mConsole;
};

#endif
