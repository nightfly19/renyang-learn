#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qwidgetlist.h>

class QAction;
class QLabel;
class QWorkspace;
class Editor;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0, const char *name = 0);

public slots:
    void newFile();
    void openFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void del();
    void about();
    void updateMenus();
    void activateWindow(int param);
    void copyAvailable(bool available);
    void updateModIndicator();

private:
    void createActions();
    void createMenus();
    void createWindowsMenu();
    void createToolBars();
    void createStatusBar();
    Editor *createEditor();
    Editor *activeEditor();

    QWorkspace *workspace;
    QLabel *readyLabel;
    QLabel *modLabel;
    QWidgetList windows;

    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *windowsMenu;
    QPopupMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *deleteAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
