
#include <qapplication.h>
#include <qcanvas.h>
#include <qfont.h>
#include <qimage.h>

class MyQCanvas:public QCanvas
{
	public:
		MyQCanvas(int w=0,int h=0):QCanvas(w,h)
		{
			// do nothing
		}
	protected:
		void advance()
		{
			QCanvas::advance();
		}
};

class View : public QCanvasView {
public:
    View(QCanvas& canvas) : QCanvasView(&canvas){
        canvas.setBackgroundPixmap(QPixmap("logo.jpg"));
        canvas.resize(370, 300);
        setFixedSize(sizeHint());

        p = new QCanvasPolygon(&canvas);  // 多邊形
        QPointArray a;
        a.setPoints(3, 50, 100, 200, 200, 50, 200);
        p->setPoints(a);
        p->setBrush(Qt::blue);
        p->setZ(10); // 深度
        p->show();  // 顯示繪圖物件

        r = new QCanvasRectangle(100, 100, 50, 100, &canvas);  // 四方形
        r->setVelocity(0.5, 0); // 移動速度
        r->setBrush(Qt::red);
        r->setZ(10);
        r->show();

        t = new QCanvasText("What will you study today? ^o^", &canvas); // 文字
        t->setFont(QFont("Helvetica", 12, QFont::Bold));
        t->setColor(Qt::blue);
        t->setZ(20);
        t->setTextFlags(AlignBottom);
        t->move(5, 20);
        t->show();
    }


protected:
    void contentsMousePressEvent(QMouseEvent *e) {
        r->move(e->pos().x(), e->pos().y());
	if (r->collidesWith(p))
		t->setText("Collision");
	else
		t->setText("That is ok!");

    }

private:
    QCanvasPolygon *p;
    QCanvasRectangle *r;
    QCanvasText *t;

};

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MyQCanvas canvas(0, 0);

    canvas.setAdvancePeriod(30);  // 移動更新間隔:
    canvas.setDoubleBuffering(true); // double buffer
    View c(canvas);

    app.setMainWidget(&c);
    c.show();

    return app.exec();
}
