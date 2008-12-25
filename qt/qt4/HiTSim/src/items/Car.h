#ifndef CAR_H
#define CAR_H

#include <QGraphicsPixmapItem>

class Car:public QGraphicsPixmapItem,public QObject
{
	public:
		Car(QGraphicsItem *parent=0);
		void rotate_direction(qreal);

	private:
		int _car_id; // 車子的編號
		double _real_pos_x,_real_pos_y; // 車子的位置
		double _real_velocity; // 車子的速度
		double _real_angle; // 車子的角度
		// 之後要加一個變數是車子的總類
		double _real_accelerator; // 車子的加速度
		double _max_real_velocity,_min_real_velocity; // 車子的最大最小速度
};

#endif // CAR_H
