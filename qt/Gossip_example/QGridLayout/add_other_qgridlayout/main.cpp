#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qslider.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QWidget *wm = new QWidget(0,"zero");
    QLabel *label;
    QGridLayout *grid = new QGridLayout(wm, 3, 3,0,-1,"first");
    QGridLayout *grid2 = new QGridLayout(0, 2, 0,0,-1,"second");

    // grid here
    grid->addColSpacing(0, 200);
    grid->addColSpacing(1, 100);

    label = new QLabel("one", wm);
    label->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    grid->addWidget(label, 0, 0);

    grid->addWidget(new QLabel("two", wm), 1, 0);

    label = new QLabel("three", wm);
    label->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    grid->addWidget(label, 2, 0);

    grid->addWidget(new QLabel("four", wm), 1, 1);

    label = new QLabel("five", wm);
    label->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    grid->addWidget(label, 2, 2);

    // grid2 here
    QPushButton quit("Quit", wm);
    QObject::connect(&quit,SIGNAL(clicked()),&app,SLOT(quit()));
    QLCDNumber lcd(2, wm);
    QSlider slider(Qt::Horizontal, wm);
    QObject::connect(&slider,SIGNAL(valueChanged(int)),&lcd,SLOT(display(int)));
    slider.setRange(0, 99);
    slider.setValue(0);
    grid2->addWidget(&quit, 0, 0);
    grid2->addWidget(&lcd, 0, 1);
    grid2->addWidget(&slider, 0, 2);

    grid->addLayout(grid2, 1, 2);
   
    app.setMainWidget(wm);
    wm->show();

    return app.exec();
}
