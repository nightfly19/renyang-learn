#include <qapplication.h>
#include <qlabel.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLabel *label = new QLabel("Hello Qt!", 0);
    app.setMainWidget(label);
    label->show();
    return app.exec();
}
