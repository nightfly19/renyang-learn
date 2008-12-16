#include <qapplication.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qlcdnumber.h>

#include <qvbox.h>

class MyWidget : public QVBox {
public:
    MyWidget(QWidget *parent=0, const char *name=0);
};


MyWidget::MyWidget(QWidget *parent, const char *name)
        : QVBox(parent, name) {
    QPushButton *quit = new QPushButton("Quit", this, "quit");

    connect(quit,SIGNAL(clicked()),qApp,SLOT(quit()));

    QLCDNumber *lcd  = new QLCDNumber(2, this, "lcd");

    QSlider * slider = new QSlider(Horizontal, this, "slider");

    connect(slider,SIGNAL(valueChanged(int)),lcd,SLOT(display(int)));
    slider->setRange(0, 99);
    slider->setValue(0);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
   
    MyWidget wm;
    app.setMainWidget(&wm);
    wm.show();
   
    return app.exec();
}
