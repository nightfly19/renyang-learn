#include <qapplication.h>
#include <qhbox.h>
#include <qslider.h>
#include <qspinbox.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QHBox *hbox = new QHBox(0);
    hbox->setCaption("Enter Your Age");
    hbox->setMargin(6);
    hbox->setSpacing(6);

    QSpinBox *spinBox = new QSpinBox(hbox);
    QSlider *slider = new QSlider(Qt::Horizontal, hbox);
    spinBox->setRange(0, 130);
    slider->setRange(0, 130);

    QObject::connect(spinBox, SIGNAL(valueChanged(int)),
                     slider, SLOT(setValue(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)),
                     spinBox, SLOT(setValue(int)));
    spinBox->setValue(35);

    app.setMainWidget(hbox);
    hbox->show();

    return app.exec();
}
