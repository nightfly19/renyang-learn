#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>
#include <qstringlist.h>

class QAction;
class QLabel;
class FindDialog;
class Spreadsheet;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0, const char *name = 0);

protected:
    void closeEvent(QCloseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void find();
    void goToCell();
    void sort();
    void about();
    void updateCellIndicators();
    void spreadsheetModified();
    void openRecentFile(int param);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileItems();
    QString strippedName(const QString &fullFileName);

    Spreadsheet *spreadsheet;
    FindDialog *findDialog;
    QLabel *locationLabel;
    QLabel *formulaLabel;
    QLabel *modLabel;
    QStringList recentFiles;
    QString curFile;
    QString fileFilters;
    bool modified;

    enum { MaxRecentFiles = 5 };
    int recentFileIds[MaxRecentFiles];

    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *selectSubMenu;
    QPopupMenu *toolsMenu;
    QPopupMenu *optionsMenu;
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
    QAction *selectRowAct;
    QAction *selectColumnAct;
    QAction *selectAllAct;
    QAction *findAct;
    QAction *goToCellAct;
    QAction *recalculateAct;
    QAction *sortAct;
    QAction *showGridAct;
    QAction *autoRecalcAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
