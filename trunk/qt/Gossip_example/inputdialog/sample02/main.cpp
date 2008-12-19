#include <qapplication.h>
#include <qmessagebox.h>
#include <qpixmap.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QMessageBox messagebox("Costumize",
                "show Qt?", QMessageBox::NoIcon,
                 QMessageBox::Yes | QMessageBox::Default,
                 QMessageBox::No | QMessageBox::Escape,
                 QMessageBox::NoButton);

    messagebox.setIconPixmap(QPixmap("dog.xpm"));

    if(messagebox.exec() == QMessageBox::Yes)
       QMessageBox::aboutQt(0, "About Qt");

    return 0;
}
