
#include <QApplication>
#include <QTabWidget>
#include <QLabel>

int main(int argc,char **argv) {
	QApplication app(argc,argv);

	QLabel *label_first = new QLabel("first");
	QLabel *label_second = new QLabel("second");

	QTabWidget w;
	w.addTab(label_first,"first");
	w.addTab(label_second,"second");
	w.setWindowTitle("test for QTabWidget");
	w.show();

	return app.exec();
}

