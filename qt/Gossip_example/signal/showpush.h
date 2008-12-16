#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <qlabel.h>
#include <qpushbutton.h>

class ShowPush : public QWidget {
    Q_OBJECT
public:
    ShowPush(QWidget *parent=0, const char *name=0);

public slots:
    void setlabelText1();
    void setlabelText2(int);

signals:
    void up10(int);

private:
    QLabel *label;
    QPushButton *btn;
    int i;
};

#endif // LCDRANGE_H
