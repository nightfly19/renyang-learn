#include <qapplication.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qstringlist.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QStringList list = QFileDialog::getOpenFileNames(
                    "Images (*.png *.xpm *.jpg)","/home",
                    0, "open file dialog Choose a file" );

    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
        QMessageBox::information(0, "Get Selected File",
             *it, "OK", "", "", 0, 1);
        ++it;
    }

    return 0;
}
