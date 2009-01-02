
#include <QtGui>

#include "coloritem.h"

ColorItem::ColorItem()
    : color(qrand() % 256, qrand() % 256, qrand() % 256)
{
    setToolTip(QString("QColor(%1, %2, %3)\n%4")	// 設定當滑鼠在物件上面時的提示
	       .arg(color.red()).arg(color.green()).arg(color.blue())
	       .arg("Click and drag this color onto the robot!"));
    setCursor(Qt::OpenHandCursor);	// 當滑鼠在物件上時,會出現一隻張開的小手
}

QRectF ColorItem::boundingRect() const	// 設定redraw的範圍
{
    return QRectF(-15.5, -15.5, 34, 34);
}

void ColorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-12, -12, 30, 30);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(color));
    painter->drawEllipse(-15, -15, 30, 30);
}

void ColorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
	event->ignore();	// 若不是按左鍵的話,則忽略它
	return;
    }

    setCursor(Qt::ClosedHandCursor);	// 指標變成握住的小手
}

void ColorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    // QLineF(目前的位置,按下的位置)
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }

    QDrag *drag = new QDrag(event->widget());	// event->widget()回傳event的來源
    QMimeData *mime = new QMimeData;	// 設定拖曳要攜帶的資料
    drag->setMimeData(mime);	// 設定drag的MimeData的資料

    static int n = 0;
    if (n++ > 2 && (qrand() % 3) == 0) {
	QImage image(":/images/head.png");
	mime->setImageData(image);	// 設定MineData為image

	drag->setPixmap(QPixmap::fromImage(image).scaled(30, 40));
        drag->setHotSpot(QPoint(15, 30));	// 設定滑鼠旁的小圖的位置
    } else {
	mime->setColorData(color);
	mime->setText(QString("#%1%2%3")
		      .arg(color.red(), 2, 16, QLatin1Char('0'))
		      .arg(color.green(), 2, 16, QLatin1Char('0'))
		      .arg(color.blue(), 2, 16, QLatin1Char('0')));

	QPixmap pixmap(34, 34);
	pixmap.fill(Qt::white);

	QPainter painter(&pixmap);
	painter.translate(15, 15);
	painter.setRenderHint(QPainter::Antialiasing);	// 讓線條有被修飾過
	paint(&painter, 0, 0);	// 設定要畫上的設備與QStyleOptionGraphicsItem
	painter.end();

	pixmap.setMask(pixmap.createHeuristicMask());

	drag->setPixmap(pixmap);		// 設定滑鼠旁的小圖示
	drag->setHotSpot(QPoint(15, 20));	// 設定澤鼠旁的小圖示的位置
    }

    drag->exec();	// 執行拖曳的功能,預設是Move的功能
    setCursor(Qt::OpenHandCursor);
}

void ColorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    setCursor(Qt::OpenHandCursor);
}
