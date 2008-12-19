#include <qapplication.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QColorDialog::setCustomColor(0, QRgb(0x0000FF));
    QColor c = QColorDialog::getColor(QColor(0, 255, 0));
    QString text;

    if(c.isValid()) {
        text.sprintf("R:%d G:%d B:%d", c.red(), c.green(), c.blue());
        QMessageBox::information(0, "Get Selected Color",
             text, "OK", "", "", 0, 1);
    }
    else {
    	QMessageBox::information(0,"Error message",
		"You cancel the color input","OK","","",0,1);
    }

    text.sprintf("%d custom colors available", QColorDialog::customCount());
    QMessageBox::information(0, "Get Selected Color",
             text, "OK", "", "", 0, 1);

    return 0;
}
