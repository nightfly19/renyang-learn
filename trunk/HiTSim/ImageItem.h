
#ifndef __IMAGEITEM_H__
#define __IMAGEITEM_H__

#include <qcanvas.h>

static const int imageRTTI = 984376;


class ImageItem: public QCanvasRectangle
{
public:
    ImageItem( QImage img, QCanvas *canvas );
    int rtti () const { return imageRTTI; }
    bool hit( const QPoint&) const;
protected:
    void drawShape( QPainter & );
private:
    QImage image;
    QPixmap pixmap;
};


#endif
