#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QPixmap>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    QMainWindow *mainWindow = new QMainWindow;
    mainWindow->setWindowTitle("QMainWindow");    
    
    // 文字編輯區 
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setFocus();

    mainWindow->setCentralWidget(textEdit);
    
    // 跳出選單
    QMenu *fileMenu = new QMenu("&File");
    QAction *fileAction = new QAction("Open..", fileMenu);
    // 快捷鍵 Ctrl+O 
    fileAction->setShortcut(Qt::CTRL + Qt::Key_O);
    
    fileMenu->addAction(fileAction);
    fileMenu->addAction("Save");
    fileMenu->addAction("Save as...");
    // 分隔線
    fileMenu->addSeparator();
    // 快捷鍵 Ctrl+X，動作連接至 QApplication 的 quit() 
    fileMenu->addAction("Close", &app, SLOT(quit()), Qt::CTRL + Qt::Key_X);
    
    QMenu *editMenu = new QMenu("&Edit");
    editMenu->addAction("Cut");
    editMenu->addAction("Copy");
    editMenu->addAction("Paste");
    
    QMenu *aboutMenu = new QMenu("&About");
    aboutMenu->addAction("About");
                
    mainWindow->menuBar()->addMenu(fileMenu);
    mainWindow->menuBar()->addMenu(editMenu);
    mainWindow->menuBar()->addMenu(aboutMenu);    

    // 工具列
    QToolBar *toolBar = new QToolBar("QToolBar");
    toolBar->addAction(QIcon("caterpillar_head.jpg"), "caterpillar");
    toolBar->addAction(QIcon("momor_head.jpg"), "momor");
    toolBar->addSeparator();
    toolBar->addAction(QIcon("bush_head.jpg"), "bush");

    mainWindow->addToolBar(toolBar);

    // 狀態列 
    QStatusBar *statusBar = mainWindow->statusBar();
    statusBar->showMessage("Status here...");

    // 停駐元件
    QDockWidget *dockWidget = new QDockWidget("QDockWidget");
    QLabel *label = new QLabel;
    label->setPixmap(QPixmap("caterpillar.jpg"));
    dockWidget->setWidget(label);

    mainWindow->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
     
    mainWindow->show();

    return app.exec();
}
