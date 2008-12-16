#include <qapplication.h>
#include <qgrid.h>
#include <qlabel.h>

class MyWidget : public QGrid {
public:
    MyWidget(int n, QWidget *parent=0, const char *name=0);
};

MyWidget::MyWidget(int n, QWidget *parent, const char *name)
        : QGrid(n, parent, name) {
    const QString texts[] = {"one", "two", "three",
                             "four", "five", "six",
                             "seven", "eight", "nine"};
    QLabel *label;

    setSpacing(2);
    setMargin(2);

    for(int i = 0; i < 8; i++) {
        label = new QLabel(texts[i], this, "label");
        label->setMinimumSize(55, 0);
        label->setAlignment(Qt::AlignCenter);
        label->setFrameStyle(QFrame::Panel + QFrame::Sunken);
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MyWidget wm(3);

    wm.resize(150, 100);
    app.setMainWidget(&wm);
    wm.show();

    return app.exec();
}
