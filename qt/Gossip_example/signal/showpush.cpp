#include "showpush.h"

ShowPush::ShowPush(QWidget *parent, const char *name)
        : QWidget(parent, name) {
    i = 0;
    label = new QLabel("Hello! Qt!", this, "label");
    label->setAlignment(AlignCenter);
    label->setGeometry(45, 20, 150, 30);

    btn = new QPushButton("Push me", this, "btn");
    btn->setGeometry(70, 70, 100, 30);

    connect(btn, SIGNAL(clicked()), SLOT(setlabelText1()));
    connect(this, SIGNAL(up10(int)), SLOT(setlabelText2(int)));

    setFocusProxy(btn);
}

void ShowPush::setlabelText1() {
    i++;
    if(i%10!=0)
        label->setText("You push the button");
    else {
        emit up10(i);
        // i = 0;
    }
}

void ShowPush::setlabelText2(int n) {
    QString text = "Push ";
    text = text +  QString::number(n) + " times";
    label->setText(text);
}
