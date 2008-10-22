#include <qpainter.h>

#include "imageeditor.h"

ImageEditor::ImageEditor(QWidget *parent, const char *name)
    : QScrollView(parent, name, WStaticContents | WNoAutoErase)
{
    curColor = black;
    zoom = 8;
    curImage.create(16, 16, 32);
    curImage.fill(qRgba(0, 0, 0, 0));
    curImage.setAlphaBuffer(true);
    resizeContents();
}

void ImageEditor::resizeContents()
{
    QSize size = zoom * curImage.size();
    if (zoom >= 3)
        size += QSize(1, 1);
    QScrollView::resizeContents(size.width(), size.height());
}

void ImageEditor::setPenColor(const QColor &newColor)
{
    curColor = newColor;
}

void ImageEditor::setImage(const QImage &newImage)
{
    if (newImage != curImage) {
        curImage = newImage.convertDepth(32);
        curImage.detach();
        resizeContents();
        updateContents();
    }
}

void ImageEditor::setZoomFactor(int newZoom)
{
    if (newZoom < 1)
        newZoom = 1;

    if (newZoom != zoom) {
        zoom = newZoom;
        resizeContents();
        updateContents();
    }
}

void ImageEditor::drawContents(QPainter *painter, int, int, int, int)
{
    if (zoom >= 3) {
        painter->setPen(colorGroup().foreground());
        for (int i = 0; i <= curImage.width(); ++i)
            painter->drawLine(zoom * i, 0,
                              zoom * i, zoom * curImage.height());
        for (int j = 0; j <= curImage.height(); ++j)
            painter->drawLine(0, zoom * j,
                              zoom * curImage.width(), zoom * j);
    }

    for (int i = 0; i < curImage.width(); ++i) {
        for (int j = 0; j < curImage.height(); ++j)
            drawImagePixel(painter, i, j);
    }
}

void ImageEditor::drawImagePixel(QPainter *painter, int i, int j)
{
    QColor color;
    QRgb rgb = curImage.pixel(i, j);

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

void ImageEditor::contentsMousePressEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton)
        setImagePixel(event->pos(), true);
    else if (event->button() == RightButton)
        setImagePixel(event->pos(), false);
}

void ImageEditor::contentsMouseMoveEvent(QMouseEvent *event)
{
    if (event->state() & LeftButton)
        setImagePixel(event->pos(), true);
    else if (event->state() & RightButton)
        setImagePixel(event->pos(), false);
}

void ImageEditor::setImagePixel(const QPoint &pos, bool opaque)
{
    int i = pos.x() / zoom;
    int j = pos.y() / zoom;

    if (curImage.rect().contains(i, j)) {
        if (opaque)
            curImage.setPixel(i, j, penColor().rgb());
        else
            curImage.setPixel(i, j, qRgba(0, 0, 0, 0));

        QPainter painter(viewport());
        painter.translate(-contentsX(), -contentsY());
        drawImagePixel(&painter, i, j);
    }
}
