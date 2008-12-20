#include <qapplication.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qdrawutil.h>

class DrawView : public QWidget {
public:
    DrawView() {
        setBackgroundColor(white);
    }

protected:
    void drawColorWheel(QPainter*);
    void paintEvent(QPaintEvent*);
};

void DrawView::drawColorWheel(QPainter *p) {
    QFont f("times", 18, QFont::Bold);
    p->setFont(f);
    p->setPen(Qt::black);
    p->setWindow(0, 0, 500, 500);             // defines coordinate system

    for (int i = 0; i < 36; i++) {            // draws 36 rotated rectangles
        QWMatrix matrix;
        matrix.translate(250.0F, 250.0F);     // move to center
        matrix.shear(0.0F, 0.3F);             // twist it
        matrix.rotate((float)i*10);           // rotate 0,10,20,.. degrees
        p->setWorldMatrix(matrix);            // use this world matrix

        QColor c;
        c.setHsv(i*10, 255, 255);             // rainbow effect
        p->setBrush(c);                       // solid fill with color c
        p->drawRect(70, -10, 80, 10);         // draw the rectangle

        QString n;
        n.sprintf("H=%d", i*10);
        p->drawText(80+70+5, 0, n);           // draw the hue number
    }
}


void DrawView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    drawColorWheel(&p);
}


int main(int argc, char **argv) {
    QApplication app(argc, argv);
    DrawView draw;
    app.setMainWidget(&draw);
    draw.show();
    return app.exec();
}
