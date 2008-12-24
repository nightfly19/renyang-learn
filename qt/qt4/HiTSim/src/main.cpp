#include "MainWindow.h"

#include <QApplication>
#include <QTime>

int main(int argc,char **argv) {

	QApplication app(argc,argv);
	qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

	MainWindow *main = new MainWindow;

	main->show();

	return app.exec();
}
