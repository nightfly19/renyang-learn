#include <qpainter.h>

#include "iconeditor.h"

IconEditor::IconEditor(QWidget *parent, const char *name)
    : QWidget(parent, name, WStaticContents)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    curColor = black;
    zoom = 8;
    image.create(16, 16, 32);
    image.fill(qRgba(0, 0, 0, 0));
    image.setAlphaBuffer(true);
}

QSize IconEditor::sizeHint() const
{
    QSize size = zoom * image.size();
    if (zoom >= 3)
        size += QSize(1, 1);
    return size;
}

void IconEditor::setPenColor(const QColor &newColor)
{
    curColor = newColor;
}

void IconEditor::setIconImage(const QImage &newImage)
{
    if (newImage != image) {
        image = newImage.convertDepth(32);
        image.detach();
        update();
        updateGeometry();
    }
}

void IconEditor::setZoomFactor(int newZoom)
{
    if (newZoom < 1)
        newZoom = 1;

    if (newZoom != zoom) {
        zoom = newZoom;
        update();
        updateGeometry();
    }
}

void IconEditor::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (zoom >= 3) {
        painter.setPen(colorGroup().foreground());
        for (int i = 0; i <= image.width(); ++i)
            painter.drawLine(zoom * i, 0,
                             zoom * i, zoom * image.height());
        for (int j = 0; j <= image.height(); ++j)
            painter.drawLine(0, zoom * j,
                             zoom * image.width(), zoom * j);
    }

    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j)
            drawImagePixel(&painter, i, j);
    }
}

void IconEditor::drawImagePixel(QPainter *painter, int i, int j)
{
    QColor color;
    QRgb rgb = image.pixel(i, j);

    if (qAlpha(rgb) == 0)
        color = colorGroup().base();
    else
        color.setRgb(rgb);

    if (zoom >= 3) {
        painter->fillRect(zoom * i + 1, zoom * j + 1,
                          zoom - 1, zoom - 1, color);
    } else {
        painter->fillRect(zoom * i, zoom * j,
                          zoom, zoom, color);
    }
}

void IconEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton)
        setImagePixel(event->pos(), true);
    else if (event->button() == RightButton)
        setImagePixel(event->pos(), false);
}

void IconEditor::mouseMoveEvent(QMouseEvent *event)
{
    if (event->state() & LeftButton)
        setImagePixel(event->pos(), true);
    else if (event->state() & RightButton)
        setImagePixel(event->pos(), false);
}

void IconEditor::setImagePixel(const QPoint &pos, bool opaque)
{
    int i = pos.x() / zoom;
    int j = pos.y() / zoom;

    if (image.rect().contains(i, j)) {
        if (opaque)
            image.setPixel(i, j, penColor().rgb());
        else
            image.setPixel(i, j, qRgba(0, 0, 0, 0));

        QPainter painter(this);
        drawImagePixel(&painter, i, j);
    }
}
