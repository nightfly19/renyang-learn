#include <qapplication.h>
#include <qlabel.h>
#include <qfontdialog.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    bool ok;

    QLabel hello("hello world", 0);
    hello.setFont(QFont( "Times", 18, QFont::Bold ));
    app.setMainWidget(&hello);
    hello.show();

    QFont font =
       QFontDialog::getFont(&ok, QFont("Times", 18, QFont::Bold));

    if(ok)
        hello.setFont(font);

    return app.exec();
}
