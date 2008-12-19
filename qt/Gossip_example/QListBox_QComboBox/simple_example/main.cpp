#include <qapplication.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qpixmap.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
 
    QVBox *vbox = new QVBox();
    QHBox *hbox = new QHBox(vbox);
    QLabel *label;

    hbox = new QHBox(vbox);
    label = new QLabel("ListBox: ", hbox);
    hbox->setSpacing(10);
    QListBox *listbox = new QListBox(hbox);
    listbox->insertItem("listbox item with text");
    listbox->insertItem(QPixmap("icon.xpm"), "listbox item with icon");
    listbox->insertItem(QPixmap("dog.xpm"), "listbox item with large icon");

    hbox = new QHBox(vbox);
    hbox->setSpacing(10);
    label = new QLabel("&ComboBox: ", hbox);
    QComboBox *comboBox = new QComboBox(FALSE, hbox);
    comboBox->insertItem("combox item with text");
    comboBox->insertItem(QPixmap("fileopen.xpm"), "combox item with icon");
    label->setBuddy(comboBox);

    app.setMainWidget(vbox);
    vbox->show();

    return app.exec();
}
