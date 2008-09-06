#ifndef HEXSPINBOX_H
#define HEXSPINBOX_H

#include <qspinbox.h>

class HexSpinBox : public QSpinBox
{
public:
    HexSpinBox(QWidget *parent, const char *name = 0);

protected:
    QString mapValueToText(int value);
    int mapTextToValue(bool *ok);
};

#endif
