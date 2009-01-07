
#include <QPainter>
#include <QPainterPath>

#include "mywidget.h"

MyWidget::MyWidget(QWidget *parent):QWidget(parent) {
	// do nothing
}

// 這裡要注意的是,當畫的圖有重疊基數次的會顯示目前的目前的圖案,而重複偶數次則會顯示背景的圖案
// 可以透過setFillRule去修改fill rule針對重疊的部分顯示背景
void MyWidget::paintEvent(QPaintEvent *) {
	QPainterPath path;
	path.addRect(20,20,60,60);	// 在(20,20)建立一個60x60的正方形

	path.moveTo(0,0);		// 把path的current point移到(0,0),因為大部分的圖形都必需由start point畫到end point,所以current point很重要
	path.cubicTo(99,0,50,50,99,99);	// 後面兩個參數是控制圖形的參數
	path.cubicTo(0,99,50,50,0,0);	// 同上
	path.addRect(0,40,100,20);	// 在(0,40)建立一個100x20的長方型

	QPainter painter(this);
	painter.translate(width()/2-50,height()/2-50);	// 設定圖在畫面的中央
	painter.fillRect(0,0,100,100,Qt::red);	// 在(0,0)填滿100x100的背景

	painter.setPen(QPen(QColor(79,106,25),1,Qt::SolidLine,Qt::FlatCap,Qt::MiterJoin));	// 設定畫筆的顏色
	painter.setBrush(QColor(122,163,39));	// 設定實心的顏色

	painter.drawPath(path);		// 開始畫圖
}
