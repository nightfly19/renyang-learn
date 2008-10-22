#include <qapplication.h>
#include <qpushbutton.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPushButton *button = new QPushButton("Quit", 0);
    QObject::connect(button, SIGNAL(clicked()),
                     &app, SLOT(quit()));
    app.setMainWidget(button);
    button->show();
    return app.exec();
}
