#ifndef ICONEDITOR_H
#define ICONEDITOR_H

#include <qimage.h>
#include <qwidget.h>

class IconEditor : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(QImage iconImage READ iconImage WRITE setIconImage)
    Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
    IconEditor(QWidget *parent = 0, const char *name = 0);

    void setPenColor(const QColor &newColor);
    QColor penColor() const { return curColor; }
    void setZoomFactor(int newZoom);
    int zoomFactor() const { return zoom; }
    void setIconImage(const QImage &newImage);
    const QImage &iconImage() const { return image; }
    QSize sizeHint() const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void drawImagePixel(QPainter *painter, int i, int j);
    void setImagePixel(const QPoint &pos, bool opaque);

    QColor curColor;
    QImage image;
    int zoom;
};

#endif
