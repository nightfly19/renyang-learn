#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QString s = QFileDialog::getOpenFileName(
                "/home", "Images (*.png *.xpm *.jpg)",
                    0, "open file dialog Choose a file" );

    if(s != NULL)
        QMessageBox::information(0, "Get Selected File",
             s, "OK", "", "", 0, 1);

    return 0;
}
