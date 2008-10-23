
#include <qapplication.h>
#include <qhbox.h>
#include <qlabel.h>

#include "ddlabel.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QHBox *hbox = new QHBox();
    hbox->setMargin(10);
    hbox->setSpacing(10);

    DDLabel *lab1 = new DDLabel(hbox, DDLabel::Sor);
    lab1->setText("Drag the Content\nto the rightest\nand drop it");

    DDLabel *lab2 = new DDLabel(hbox, DDLabel::Sor);
    lab2->setPixmap(QPixmap("logo.jpg"));

    DDLabel *lab3 = new DDLabel(hbox, DDLabel::Des);
    lab3->setText("Drop here");

    app.setMainWidget(hbox);
    hbox->show();

    return app.exec();
}
