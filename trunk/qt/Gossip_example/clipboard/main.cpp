#include <qapplication.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qdragobject.h>
#include <qclipboard.h>

class ClipLabel : public QLabel {
public:
    enum labelType {Sor, Des};
    ClipLabel(QWidget *parent, int type) : QLabel(parent), labeltype(type) {
        setFrameStyle(QFrame::Sunken + QFrame::WinPanel);
        setBackgroundColor(Qt::green);
    }

protected:
    void mousePressEvent(QMouseEvent *) {
        if(labeltype == Des) {
            QString str;
            QPixmap pm;
            // 取出剪貼簿物件
            QMimeSource *clip = qApp->clipboard()->data();

            // 判斷類型並解碼
            if(QTextDrag::decode(clip, str))
                setText(str);
            else if(QImageDrag::decode(clip, pm))
                setPixmap(pm);
        }
    }

    void mouseDoubleClickEvent(QMouseEvent *) {
        QDragObject *drobj = 0; // 這個物件會自動清除

        if(pixmap()) // 如果有圖片
            drobj = new QImageDrag(pixmap()->convertToImage(), this);
        else if(text()) // 如果有文字
            drobj = new QTextDrag(text(), this);

        // 指定至剪貼簿
        if(drobj) qApp->clipboard()->setData(drobj);
    }

private:
    int labeltype;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QHBox *hbox = new QHBox();
    hbox->setMargin(10);
    hbox->setSpacing(10);
    ClipLabel *lab1 = new ClipLabel(hbox, ClipLabel::Sor);
    lab1->setPixmap(QPixmap("wuwu.jpg"));

    ClipLabel *lab2 = new ClipLabel(hbox, ClipLabel::Sor);
    lab2->setText("double \nclick to\ncopy");

    ClipLabel *lab3 = new ClipLabel(hbox, ClipLabel::Des);
    lab3->setText("Click me");

    app.setMainWidget(hbox);
    hbox->show();

    return app.exec();
}
