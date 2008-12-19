#include <qapplication.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QVBox *box = new QVBox();
    box->setGeometry(200, 200, 200, 150);

    QCheckBox *checkbox = new QCheckBox(box);
    checkbox->setChecked(true);
    checkbox->setText("item 1");
    checkbox = new QCheckBox("item 2", box);

    QHBox *horizontal_box = new QHBox(box);

    QButtonGroup *group = new QButtonGroup(1, QGroupBox::Horizontal, "QButton Group Horizontal", horizontal_box);
    QRadioButton *radiobutton = new QRadioButton("rdbutton 1", group);
    radiobutton = new QRadioButton("rdbutton 2", group);
    radiobutton = new QRadioButton("rdbutton 3", group);

    group = new QButtonGroup(1,QGroupBox::Vertical,"QButton Group Vertical",horizontal_box);
    radiobutton = new QRadioButton("vbutton 1",group);
    radiobutton = new QRadioButton("vbutton 2",group);
    radiobutton = new QRadioButton("vbutton 3",group);

    app.setMainWidget(box);
    box->show();

    return app.exec();
}
