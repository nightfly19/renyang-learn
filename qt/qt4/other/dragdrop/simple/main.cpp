#include <QApplication>
#include <QMainWindow>
#include "ImageLabel.h"
#include <QTextEdit>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    ImageLabel *imgLabel = new ImageLabel;
    imgLabel->setWindowTitle("Drag & Drop");
    imgLabel->setText("<center>Drag & Drop Image here</center>");
    imgLabel->resize(300, 200);
    imgLabel->show();

    return app.exec();
}
