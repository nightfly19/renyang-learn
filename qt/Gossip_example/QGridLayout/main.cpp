#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QWidget *wm = new QWidget;
    QLabel *label;
    QGridLayout *grid = new QGridLayout(wm, 3, 3);

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

    app.setMainWidget(wm);
    wm->show();

    return app.exec();
}
