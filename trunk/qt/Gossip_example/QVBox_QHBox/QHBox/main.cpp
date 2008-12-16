#include <qapplication.h> 
#include <qpushbutton.h> 
#include <qslider.h> 
#include <qlcdnumber.h> 

#include <qhbox.h> 

int main(int argc, char **argv) { 
    QApplication app(argc, argv); 

    QHBox box; 
    box.resize(200, 50); 

    QPushButton quit("Quit", &box); 

    QObject::connect(&quit,SIGNAL(clicked()),&app,SLOT(quit()));

    QLCDNumber lcd(2, &box);
    QSlider slider(Qt::Horizontal, &box);
    slider.setRange(0, 99);
    slider.setValue(0);

    QObject::connect(&slider,SIGNAL(valueChanged(int)),&lcd,SLOT(display(int)));

    app.setMainWidget(&box); 
    box.show(); 

    return app.exec(); 
}
