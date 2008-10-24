
#include "mybutton.h"

MyPushButton::MyPushButton(QWidget *parent,const char *name, const char *text):QPushButton(parent,text){
	setPixmap(QPixmap("cater2.png"));
	setToggleType(Toggle); // 設定按壓後,不會彈起來
}
	
