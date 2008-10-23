
#include "ddlabel.h"

DDLabel::DDLabel(QWidget *parent, int type) : QLabel(parent), labeltype(type) {
	setBackgroundColor(Qt::green);
	setAlignment(AlignCenter);
	setAcceptDrops(true); // 可接收Drop
}

void DDLabel::dragEnterEvent(QDragEnterEvent *e) {
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
void DDLabel::dragLeaveEvent(QDragLeaveEvent *) {
        setBackgroundColor(Qt::green);
}

void DDLabel::dropEvent(QDropEvent *e) {
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

void DDLabel::mousePressEvent(QMouseEvent *e) {
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

            // 始拖放動作
            if(drobj)
                drobj->dragCopy();
        }
}   

