#include "MainWindow.h"
#include "CentralWidget.h"

#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent,Qt::WindowFlags flags):QMainWindow(parent,flags){
	
	this->setWindowTitle("HiTSim");

	// 建立主畫面
	CentralWidget *centralwidget = new CentralWidget(this);

	this->setCentralWidget(centralwidget);

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
	aboutMenu->addAction("About Qt",this,SLOT(aboutQt()));
	aboutMenu->addAction("About HiTSim",this,SLOT(aboutHiTSim()));
	this->menuBar()->addMenu(aboutMenu);
	// 建立工具列 - 建立第二個選單項目About - end
	// 建立建立工具列 - end
	
	// 建立工具列 - start
	QToolBar *toolBar = new QToolBar("QToolBar");
	QAction *toolBarAction = new QAction(QIcon(":open.png"),"open",toolBar);
	toolBarAction->setEnabled(false);	// 設定成按扭不能按,因為,還沒有功能
	toolBar->addAction(toolBarAction);
	toolBarAction = new QAction(QIcon(":icon_save"),"save",toolBar);
	toolBarAction->setEnabled(false);	// 設定成不能按,因為功能還沒有做好
	toolBar->addAction(toolBarAction);
	toolBar->addAction(QIcon(":/icon_clock.jpg"),"addCar",centralwidget,SLOT(addCar()));
	toolBar->addSeparator();
	toolBar->addAction(QIcon(":/icon_close.jpg"),"close",this,SLOT(close()));
	this->addToolBar(toolBar);
	// 建立工具列 - end
	
	// 建立狀態列 - start
	QStatusBar *statusBar = this->statusBar();
	statusBar->showMessage("Status here...");
	// 建立狀態列 - end
	
	// 建立停駐元件 - start
	QDockWidget *dockWidget = new QDockWidget("QDockWidget");
	QLabel *label = new QLabel(dockWidget);
	label->setPixmap(QPixmap(":/caterpillar.jpg"));
	dockWidget->setWidget(label);

	this->addDockWidget(Qt::RightDockWidgetArea,dockWidget);
	// 建立停駐元件 - end
	
	// 設定最大化 - start
	showMaximized();
	// 設定最大化 - end
}

void MainWindow::aboutQt() const {
	QMessageBox::aboutQt(0);
}

void MainWindow::aboutHiTSim() const {
	QMessageBox::information(0,"About HiTSim","Simulation for the vehicles");
}
