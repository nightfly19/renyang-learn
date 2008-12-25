
#ifndef __POS_H__
#define __POS_H__

// 此類別是用來記錄車子的位置(x,y) 


class Pos
{
public:
	Pos(double x = 0,double y = 0);

	double X(){	return px;	};
	double Y(){	return py;	};

	void setY(double y){	py = y;	};
	void setX(double x){	px = x;	};

	void operator =(Pos& p){	setX(p.X());	setY(p.Y());	};

private:	
	double px;
	double py;

};

#endif
