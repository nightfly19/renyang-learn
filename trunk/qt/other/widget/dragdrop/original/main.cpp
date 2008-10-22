#include <qapplication.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qdragobject.h>
#include <qclipboard.h>
#include <qdatetime.h>

class DDLabel : public QLabel {
public:
    enum labelType {Sor, Des};

    DDLabel(QWidget *parent, int type) : QLabel(parent), labeltype(type) {
        setBackgroundColor(Qt::green);
        setAlignment(AlignCenter);
        setAcceptDrops(true); // 可接收Drop
    }

protected:
    void dragEnterEvent(QDragEnterEvent *e) {
        if(e->source() == this) return;

        if(labeltype == Des) {
            // 如果可解析物件: 影像或文字
            if(QImageDrag::canDecode(e) || QTextDrag::canDecode(e)) {
                e->accept(); // 接收拖放物件
                setBackgroundColor(Qt::white);
            }
        }
    }

    // 還原背景顏色
    void dragLeaveEvent(QDragLeaveEvent *) {
        setBackgroundColor(Qt::green);
    }

    void dropEvent(QDropEvent *e) {
        QString str;
        QPixmap pm;

        QMimeSource *drop = e;

        // 解析物件
        if(QTextDrag::decode(drop, str))
            setText(str);
        else if(QImageDrag::decode(drop, pm))
            setPixmap(pm);       

        setBackgroundColor(Qt::green);
    }

    void mousePressEvent(QMouseEvent *e) {
        if(e->button() == LeftButton) {
            if(labeltype == Des)
                return;

            QDragObject *drobj = 0;

            if(pixmap()) { // 如果有圖片
                // 產生QDragObject
                drobj = new QImageDrag(pixmap()->convertToImage(), this);

                QPixmap pm;
                // 產生縮圖
                pm.convertFromImage(
                   pixmap()->convertToImage().smoothScale(25, 25));
                // 拖曳時的顯示圖形與游標位移
                drobj->setPixmap(pm, QPoint(-14, -20));
            }
            else if(text()) { // 如果有文字
                drobj = new QTextDrag(text(), this);
                drobj->setPixmap(QPixmap("text.gif"), QPoint(-25, -15));
            }

            // ?#125;始拖放動作
            if(drobj)
                drobj->dragCopy();
        }
    }   

private:
    int labeltype;
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QHBox *hbox = new QHBox();
    hbox->setMargin(10);
    hbox->setSpacing(10);

    DDLabel *lab1 = new DDLabel(hbox, DDLabel::Sor);
    lab1->setText("Drag the Content\nto the rightest\nand drop it");

    DDLabel *lab2 = new DDLabel(hbox, DDLabel::Sor);
    lab2->setPixmap(QPixmap("logo.jpg"));

    DDLabel *lab3 = new DDLabel(hbox, DDLabel::Des);
    lab3->setText("Drop here");

    app.setMainWidget(hbox);
    hbox->show();

    return app.exec();
}
