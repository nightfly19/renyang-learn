#include <qvalidator.h>

#include "hexspinbox.h"

HexSpinBox::HexSpinBox(QWidget *parent, const char *name)
    : QSpinBox(parent, name)
{
    QRegExp regExp("[0-9A-Fa-f]+");
    setValidator(new QRegExpValidator(regExp, this));
    setRange(0, 255);
}

QString HexSpinBox::mapValueToText(int value)
{
    return QString::number(value, 16).upper();
}

int HexSpinBox::mapTextToValue(bool *ok)
{
    return text().toInt(ok, 16);
}
