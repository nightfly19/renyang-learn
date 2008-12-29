#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// 建立主要畫面
class MainWindow:public QMainWindow {
Q_OBJECT
	public:
		MainWindow(QWidget *parent = 0,Qt::WindowFlags flags = 0);
	
	private slots:
		void aboutQt() const;
		void aboutHiTSim() const;

};

#endif // MAINWINDOW_H
