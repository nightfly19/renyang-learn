#include <qapplication.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qdrawutil.h>

class DrawView : public QWidget {
public:
    DrawView() {
        setBackgroundColor(white);  // 背景為白色
    }

protected:
    void drawShapes(QPainter *p, int sx, int sy);
    void paintEvent(QPaintEvent*);
};

void DrawView::drawShapes(QPainter *p, int sx, int sy) {
    int x = sx, y = sy;
    int w = 80, h = 35;

    p->save();  // 儲存繪圖裝置狀態,因為要修改p,所以,在修改成前先儲存先前狀態
   
    p->setPen(QPen(Qt::blue, 4, DotLine));  // 藍色、4pixel大的點狀畫筆
    p->drawLine(x, y, x+w, y); // 由(x, y)繪到(x+w, y)
    x += (w+10);

    p->setPen(SolidLine); // 實線
    p->setBrush(QBrush(Qt::green, DiagCrossPattern)); // 綠色的方格畫刷
    p->drawRect(x, y, w, h); // 畫方塊，左上座標(x, y)，寬高(w, h)
    x += (w+10);

    p->setBrush(Qt::yellow); // 實心黃色畫刷
    p->drawRoundRect(x, y, w, h, 0, 20); // 圓角方塊，圓角半徑20pixel
    x += (w+10);

    p->drawEllipse(x, y, w, h); // 以方塊為範圍的橢圓形
    x = sx;
    y += (h+10);
    p->drawPie(x, y, w, h, 45*16, 300*16); // 畫缺角的圓
    p->drawArc(x+3, y, w, h, 45*16, -60*16); // 畫弧
    x += (w+10);
    p->drawChord(x, y, w, h, -45, 160*16); // 畫圓切
    x += (w+10);

    p->drawPixmap(x, y, QPixmap("pengiun.xpm")); // 畫圖片
    x += (w+10);

    QBrush b(Qt::lightGray);
    qDrawWinButton(p, x, y, w, h, colorGroup(), false, &b);  // 畫一個按鈕,qDrawWinButton不是QPainter的函數嗎?為什麼可以直接呼叫?

    x = sx;
    y += (h+10);

    p->setPen(Qt::red);
    p->setBrush(NoBrush); // 無畫刷
    p->drawRect(x, y, 2*w-12 , 3*h);  // 所以是空心方塊
    // 填圖
    p->drawTiledPixmap(x+2, y+2, 2*w-16, 3*h-4, QPixmap("pengiun.xpm"));

    p->restore(); // 復原繪圖裝置狀態,把之前存下來的p參數值還原
}

// 在paintEvent事件函式中呼叫，就不怕圖形被覆蓋了. 當呼叫repaint()或update()時,此函數就會被執行,或是視窗有任何變動時,update()就會直接被系統執行
void DrawView::paintEvent(QPaintEvent*) {
    QPainter p(this);
    drawShapes(&p, 10, 10);
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    DrawView draw;
    app.setMainWidget(&draw);
    draw.show();
    return app.exec();
}
