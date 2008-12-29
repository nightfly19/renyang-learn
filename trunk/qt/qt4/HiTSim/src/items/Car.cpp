#include "Car.h"

#include <math.h>

Car::Car(QGraphicsItem *parent):QGraphicsPixmapItem(parent),_real_velocity(0.0),_real_angle(0.0)
{
	setPixmap(QPixmap(":car_b.png")); // 建立圖示
	setPos(-(pixmap().width()/2),-(pixmap().height()/2)); // 設定車子小座標的原點剛好是在車子圖形的中央
	startTimer(1000); // 每1000ms呼叫timerEvent一次
}

void Car::rotate_direction(double angle)
{
	rotate(angle);
}
