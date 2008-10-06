
#include <qimage.h>
#include <qpainter.h>

#include "ImageItem.h"

ImageItem::ImageItem( QImage img, QCanvas *canvas )
    : QCanvasRectangle( canvas ), image(img)
{
    setSize( image.width(), image.height() );

#if !defined(Q_WS_QWS)
    pixmap.convertFromImage(image, OrderedAlphaDither);
#endif
}


void ImageItem::drawShape( QPainter &p )
{
// On Qt/Embedded, we can paint a QImage as fast as a QPixmap,
// but on other platforms, we need to use a QPixmap.
#if defined(Q_WS_QWS)
    p.drawImage( int(x()), int(y()), image, 0, 0, -1, -1, OrderedAlphaDither );
#else
    p.drawPixmap( int(x()), int(y()), pixmap );
#endif
}

bool ImageItem::hit( const QPoint &p ) const
{
    int ix = p.x()-int(x());
    int iy = p.y()-int(y());
    if ( !image.valid( ix , iy ) )
	return FALSE;
    QRgb pixel = image.pixel( ix, iy );
    return qAlpha( pixel ) != 0;
}

