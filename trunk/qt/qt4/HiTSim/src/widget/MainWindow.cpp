#include "MainWindow.h"
#include "MainView.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent,Qt::WindowFlags flags):QMainWindow(parent,flags){
	
	this->setWindowTitle("HiTSim");

	// 建立主畫面
	MainView *view = new MainView(this);

	this->setCentralWidget(view);

	// 建立工具列 - start
	// 建立工具列 - 建立第一個選單項目File - start
	QMenu *fileMenu = new QMenu("&File",this);
	QAction *fileAction = new QAction("Open..",fileMenu);
	fileAction->setEnabled(false);
	// 快捷鍵 Ctrl+O:當焦點在選單時,ctrl+o才會有效果,且下面兩行在一起才會有效果
	fileAction->setShortcut(Qt::CTRL + Qt::Key_O);
	fileMenu->addAction(fileAction);
	
	fileAction = new QAction("Save",fileMenu);
	fileAction->setEnabled(false);
	fileMenu->addAction(fileAction);

	// 快捷鍵 Ctrl+X，動作連接至 QApplication 的 quit()

	// 建立分格線
	fileMenu->addSeparator();
	
	fileMenu->addAction("Close",this,SLOT(close()),Qt::CTRL + Qt::Key_X);
	// 把建立好的選單加入主畫面中
	this->menuBar()->addMenu(fileMenu);
	// 建立工具列 - 建立第一個選單項目File - end
	// 建立工具列 - 建立第二個選單項目About - start
	QMenu *aboutMenu = new QMenu("&About");
	aboutMenu->addAction("About");
	this->menuBar()->addMenu(aboutMenu);
	// 建立工具列 - 建立第二個選單項目About - end
	// 建立建立工具列 - end
	
	// 建立工具列 - start
	QToolBar *toolBar = new QToolBar("QToolBar");
	toolBar->addAction(QIcon("icon/open.png"),"open");
	toolBar->addAction(QIcon("icon/icon_save.jpg"),"save");
	toolBar->addAction(QIcon("icon/icon_clock.jpg"),"addCar",view,SLOT(addCar()));
	toolBar->addSeparator();
	toolBar->addAction(QIcon("icon/icon_close.jpg"),"close",this,SLOT(close()));
	this->addToolBar(toolBar);
	// 建立工具列 - end
	
	// 建立狀態列 - start
	QStatusBar *statusBar = this->statusBar();
	statusBar->showMessage("Status here...");
	// 建立狀態列 - end
	
	// 建立停駐元件 - start
	QDockWidget *dockWidget = new QDockWidget("QDockWidget");
	QLabel *label = new QLabel(dockWidget);
	label->setPixmap(QPixmap("images/caterpillar.jpg"));
	dockWidget->setWidget(label);

	this->addDockWidget(Qt::RightDockWidgetArea,dockWidget);
	// 建立停駐元件 - end
}
