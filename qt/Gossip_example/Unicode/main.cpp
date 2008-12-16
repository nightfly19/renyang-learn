#include <qapplication.h>
#include <qlabel.h>
#include <qtextcodec.h>

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QLabel wm("", 0);

    QTextCodec *codec = QTextCodec::codecForName("zh_TW.Big5");
    wm.setText(codec->toUnicode("中文測試"));
    wm.setAlignment(Qt::AlignCenter);

    wm.resize(150, 20);
    app.setMainWidget(&wm);
    wm.show();

    return app.exec();
}
