
#include "caterpillar.h"

Caterpillar::Caterpillar(QCanvas *canvas):QCanvasSprite(0,canvas){
	static QCanvasPixmapArray ani;
	// 讀入一組圖片
	ani.setImage(0, new QCanvasPixmap("cater1.png"));
	ani.setImage(1, new QCanvasPixmap("cater2.png"));
	// setup圖片組
	setSequence(&ani);
	// 循環播放
	setFrameAnimation(QCanvasSprite::Cycle);
}

