
#include <qapplication.h>
#include <qcanvas.h>
#include <qimage.h>

class Caterpillar : public QCanvasSprite {
public:
    Caterpillar(QCanvas *canvas) : QCanvasSprite(0, canvas) {
        static QCanvasPixmapArray ani;
        // 讀入一組圖片
        ani.setImage(0, new QCanvasPixmap("cater1.png"));
        ani.setImage(1, new QCanvasPixmap("cater2.png"));
        // setup圖片組
        setSequence(&ani);
        // 循環播放
        setFrameAnimation(QCanvasSprite::Cycle);
    }
};

class View : public QCanvasView {
public:
    View(QCanvas& canvas) : QCanvasView(&canvas){
        canvas.resize(300, 200);
        setFixedSize(sizeHint());

        Caterpillar *cater1 = new Caterpillar(&canvas);
        cater1->move(250, 50);
        cater1->setVelocity(-1, 0);
        cater1->setZ(10);
        cater1->show();

        Caterpillar *cater2 = new Caterpillar(&canvas);
        cater2->move(250, 100);
        cater2->setVelocity(-1.5, 0);
        cater2->setZ(10);
        cater2->show();
     }
};

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QCanvas canvas(0, 0);

    canvas.setAdvancePeriod(500);  // 移動更新間隔:
    canvas.setDoubleBuffering(true); // double buffer
    View c(canvas);

    app.setMainWidget(&c);
    c.show();

    return app.exec();
} 
