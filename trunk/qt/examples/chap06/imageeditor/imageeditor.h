#ifndef IMAGEEDITOR_H
#define IMAGEEDITOR_H

#include <qimage.h>
#include <qscrollview.h>

class ImageEditor : public QScrollView
{
    Q_OBJECT
    Q_PROPERTY(QColor penColor READ penColor WRITE setPenColor)
    Q_PROPERTY(QImage image READ image WRITE setImage)
    Q_PROPERTY(int zoomFactor READ zoomFactor WRITE setZoomFactor)

public:
    ImageEditor(QWidget *parent = 0, const char *name = 0);

    void setPenColor(const QColor &newColor);
    QColor penColor() const { return curColor; }
    void setZoomFactor(int newZoom);
    int zoomFactor() const { return zoom; }
    void setImage(const QImage &newImage);
    const QImage &image() const { return curImage; }

protected:
    void contentsMousePressEvent(QMouseEvent *event);
    void contentsMouseMoveEvent(QMouseEvent *event);
    void drawContents(QPainter *painter, int x, int y,
                      int width, int height);

private:
    void drawImagePixel(QPainter *painter, int i, int j);
    void setImagePixel(const QPoint &pos, bool opaque);
    void resizeContents();

    QColor curColor;
    QImage curImage;
    int zoom;
};

#endif
