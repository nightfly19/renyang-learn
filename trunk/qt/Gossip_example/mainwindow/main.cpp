#include <qapplication.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qtextedit.h>
#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qmessagebox.h>
#include <qstatusbar.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QMainWindow *mw = new QMainWindow;

    // 文字編輯區域
    QTextEdit *edit = new QTextEdit(mw, "editor");
    edit->setFocus();
    mw->setCaption("Main Window");
    mw->setCentralWidget(edit);

    // 選單
    QPopupMenu *file = new QPopupMenu(mw);
    mw->menuBar()->insertItem("&File", file);
    file->insertItem("&Open", mw, SLOT(whatsThis()), Qt::Key_F2);
    file->insertItem("&Save", mw, SLOT(whatsThis()), Qt::CTRL + Qt::Key_S);
   
    QPopupMenu *help = new QPopupMenu(mw);
    mw->menuBar()->insertItem("&Help", help);
    help->insertItem("&About", mw, SLOT(whatsThis()), Qt::Key_F1);

    // 工具列
    QToolBar *fileTools = new QToolBar(mw, "file operations");
    fileTools->setLabel("File ToolBar" );
    new QToolButton(QPixmap("openIcon.xpm"), "Open File", "Open",
          &app, SLOT(quit()), fileTools, "open file");
    new QToolButton(QPixmap("saveIcon.xpm"), "Save File", "Save",
          &app, SLOT(quit()), fileTools, "save file");

    // 狀態列
    QStatusBar *statusbar = mw->statusBar();
    statusbar->message("Status Bar");

    app.setMainWidget(mw);
    mw->show();

    return app.exec();
}
