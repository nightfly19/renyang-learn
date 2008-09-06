#include <qaction.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qinputdialog.h>
#include <qpainter.h>
#include <qpopupmenu.h>

#include <algorithm>
using namespace std;

#include "diagramview.h"
#include "propertiesdialog.h"

DiagramView::DiagramView(QCanvas *canvas, QWidget *parent,
                         const char *name)
    : QCanvasView(canvas, parent, name)
{
    pendingItem = 0;
    activeItem = 0;
    minZ = 0;
    maxZ = 0;
    createActions();
}

void DiagramView::createActions()
{
    cutAct = new QAction(tr("Cu&t"), tr("Ctrl+X"), this);
    cutAct->setIconSet(QPixmap::fromMimeSource("cut.png"));
    connect(cutAct, SIGNAL(activated()), this, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), tr("Ctrl+C"), this);
    copyAct->setIconSet(QPixmap::fromMimeSource("copy.png"));
    connect(copyAct, SIGNAL(activated()), this, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), tr("Ctrl+V"), this);
    pasteAct->setIconSet(QPixmap::fromMimeSource("paste.png"));
    connect(pasteAct, SIGNAL(activated()), this, SLOT(paste()));

    deleteAct = new QAction(tr("&Delete"), tr("Del"), this);
    deleteAct->setIconSet(QPixmap::fromMimeSource("delete.png"));
    connect(deleteAct, SIGNAL(activated()), this, SLOT(del()));

    propertiesAct = new QAction(tr("&Properties..."), 0, this);
    connect(propertiesAct, SIGNAL(activated()),
            this, SLOT(properties()));

    addBoxAct = new QAction(tr("Add &Box"), 0, this);
    addBoxAct->setIconSet(QPixmap::fromMimeSource("box.png"));
    connect(addBoxAct, SIGNAL(activated()), this, SLOT(addBox()));

    addLineAct = new QAction(tr("Add &Line"), 0, this);
    addLineAct->setIconSet(QPixmap::fromMimeSource("line.png"));
    connect(addLineAct, SIGNAL(activated()), this, SLOT(addLine()));

    bringToFrontAct = new QAction(tr("Bring to &Front"), 0, this);
    bringToFrontAct->setIconSet(
            QPixmap::fromMimeSource("bringtofront.png"));
    connect(bringToFrontAct, SIGNAL(activated()),
            this, SLOT(bringToFront()));

    sendToBackAct = new QAction(tr("Send to &Back"), 0, this);
    sendToBackAct->setIconSet(
            QPixmap::fromMimeSource("sendtoback.png"));
    connect(sendToBackAct, SIGNAL(activated()),
            this, SLOT(sendToBack()));
}

void DiagramView::contentsContextMenuEvent(QContextMenuEvent *event)
{
    QPopupMenu contextMenu(this);
    if (activeItem) {
        cutAct->addTo(&contextMenu);
        copyAct->addTo(&contextMenu);
        deleteAct->addTo(&contextMenu);
        contextMenu.insertSeparator();
        bringToFrontAct->addTo(&contextMenu);
        sendToBackAct->addTo(&contextMenu);
        contextMenu.insertSeparator();
        propertiesAct->addTo(&contextMenu);
    } else {
        pasteAct->addTo(&contextMenu);
        contextMenu.insertSeparator();
        addBoxAct->addTo(&contextMenu);
        addLineAct->addTo(&contextMenu);
    }
    contextMenu.exec(event->globalPos());
}

void DiagramView::addBox()
{
    addItem(new DiagramBox(canvas()));
}

void DiagramView::addLine()
{
    addItem(new DiagramLine(canvas()));
}

void DiagramView::addItem(QCanvasItem *item)
{
    delete pendingItem;
    pendingItem = item;
    setActiveItem(0);
    setCursor(crossCursor);
}

void DiagramView::contentsMousePressEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton && pendingItem) {
        pendingItem->move(event->pos().x(), event->pos().y());
        showNewItem(pendingItem);
        pendingItem = 0;
        unsetCursor();
    } else {
        QCanvasItemList items = canvas()->collisions(event->pos());
        if (items.empty())
            setActiveItem(0);
        else
            setActiveItem(*items.begin());
    }
    lastPos = event->pos();
}

void DiagramView::contentsMouseMoveEvent(QMouseEvent *event)
{
    if (event->state() & LeftButton) {
        if (activeItem) {
            activeItem->moveBy(event->pos().x() - lastPos.x(),
                               event->pos().y() - lastPos.y());
            lastPos = event->pos();
            canvas()->update();
        }
    }
}

void DiagramView::contentsMouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton && activeItem
            && activeItem->rtti() == DiagramBox::RTTI) {
        DiagramBox *box = (DiagramBox *)activeItem;
        bool ok;

        QString newText = QInputDialog::getText(
                tr("Diagram"), tr("Enter new text:"),
                QLineEdit::Normal, box->text(), &ok, this);
        if (ok) {
            box->setText(newText);
            canvas()->update();
        }
    }
}

void DiagramView::bringToFront()
{
    if (activeItem) {
        ++maxZ;
        activeItem->setZ(maxZ);
        canvas()->update();
    }
}

void DiagramView::sendToBack()
{
    if (activeItem) {
        --minZ;
        activeItem->setZ(minZ);
        canvas()->update();
    }
}

void DiagramView::cut()
{
    copy();
    del();
}

void DiagramView::copy()
{
    if (activeItem) {
        QString str;

        if (activeItem->rtti() == DiagramBox::RTTI) {
            DiagramBox *box = (DiagramBox *)activeItem;
            str = QString("DiagramBox %1 %2 %3 %4 %5")
                  .arg(box->width())
                  .arg(box->height())
                  .arg(box->pen().color().name())
                  .arg(box->brush().color().name())
                  .arg(box->text());
        } else if (activeItem->rtti() == DiagramLine::RTTI) {
            DiagramLine *line = (DiagramLine *)activeItem;
            QPoint delta = line->endPoint() - line->startPoint();
            str = QString("DiagramLine %1 %2 %3")
                  .arg(delta.x())
                  .arg(delta.y())
                  .arg(line->pen().color().name());
        }
        QApplication::clipboard()->setText(str);
    }
}

void DiagramView::paste()
{
    QString str = QApplication::clipboard()->text();
    QTextIStream in(&str);
    QString tag;

    in >> tag;
    if (tag == "DiagramBox") {
        int width;
        int height;
        QString lineColor;
        QString fillColor;
        QString text;

        in >> width >> height >> lineColor >> fillColor;
        text = in.read();

        DiagramBox *box = new DiagramBox(canvas());
        box->move(20, 20);
        box->setSize(width, height);
        box->setText(text);
        box->setPen(QColor(lineColor));
        box->setBrush(QColor(fillColor));
        showNewItem(box);
    } else if (tag == "DiagramLine") {
        int deltaX;
        int deltaY;
        QString lineColor;

        in >> deltaX >> deltaY >> lineColor;

        DiagramLine *line = new DiagramLine(canvas());
        line->move(20, 20);
        line->setPoints(0, 0, deltaX, deltaY);
        line->setPen(QColor(lineColor));
        showNewItem(line);
    }
}

void DiagramView::del()
{
    if (activeItem) {
        QCanvasItem *item = activeItem;
        setActiveItem(0);
        delete item;
        canvas()->update();
    }
}

void DiagramView::properties()
{
    if (activeItem) {
        PropertiesDialog dialog;
        dialog.exec(activeItem);
    }
}

void DiagramView::showNewItem(QCanvasItem *item)
{
    setActiveItem(item);
    bringToFront();
    item->show();
    canvas()->update();
}

void DiagramView::setActiveItem(QCanvasItem *item)
{
    if (item != activeItem) {
        if (activeItem)
            activeItem->setActive(false);
        activeItem = item;
        if (activeItem)
            activeItem->setActive(true);
        canvas()->update();
    }
}

const int Margin = 2;

void drawActiveHandle(QPainter &painter, const QPoint &center)
{
    painter.setPen(Qt::black);
    painter.setBrush(Qt::gray);
    painter.drawRect(center.x() - Margin, center.y() - Margin,
                     2 * Margin + 1, 2 * Margin + 1);
}

DiagramBox::DiagramBox(QCanvas *canvas)
    : QCanvasRectangle(canvas)
{
    setSize(100, 60);
    setPen(black);
    setBrush(white);
    str = "Text";
}

DiagramBox::~DiagramBox()
{
    hide();
}

void DiagramBox::setText(const QString &newText)
{
    str = newText;
    update();
}

void DiagramBox::drawShape(QPainter &painter)
{
    QCanvasRectangle::drawShape(painter);
    painter.drawText(rect(), AlignCenter, text());
    if (isActive()) {
        drawActiveHandle(painter, rect().topLeft());
        drawActiveHandle(painter, rect().topRight());
        drawActiveHandle(painter, rect().bottomLeft());
        drawActiveHandle(painter, rect().bottomRight());
    }
}

QRect DiagramBox::boundingRect() const
{
    return QRect((int)x() - Margin, (int)y() - Margin,
                 width() + 2 * Margin, height() + 2 * Margin);
}

DiagramLine::DiagramLine(QCanvas *canvas)
    : QCanvasLine(canvas)
{
    setPoints(0, 0, 0, 99);
}

DiagramLine::~DiagramLine()
{
    hide();
}

void DiagramLine::drawShape(QPainter &painter)
{
    QCanvasLine::drawShape(painter);
    if (isActive()) {
        drawActiveHandle(painter, startPoint() + offset());
        drawActiveHandle(painter, endPoint() + offset());
    }
}

QPointArray DiagramLine::areaPoints() const
{
    const int Extra = Margin + 1;
    QPointArray points(6);
    QPoint pointA = startPoint() + offset();
    QPoint pointB = endPoint() + offset();

    if (pointA.x() > pointB.x())
        swap(pointA, pointB);

    points[0] = pointA + QPoint(-Extra, -Extra);
    points[1] = pointA + QPoint(-Extra, +Extra);
    points[3] = pointB + QPoint(+Extra, +Extra);
    points[4] = pointB + QPoint(+Extra, -Extra);
    if (pointA.y() > pointB.y()) {
        points[2] = pointA + QPoint(+Extra, +Extra);
        points[5] = pointB + QPoint(-Extra, -Extra);
    } else {
        points[2] = pointB + QPoint(-Extra, +Extra);
        points[5] = pointA + QPoint(+Extra, -Extra);
    }
    return points;
}
