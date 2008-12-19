#ifndef SIMREAD_H
#define SIMREAD_H

#include <qvbox.h>

class QTextEdit;
class QPushButton;

class SimReader : public QVBox {
    Q_OBJECT

public:
    SimReader(QWidget *parent = 0, const char *name = 0);

protected:
    QTextEdit *output;
    QPushButton *openButton;

protected slots:
    void readTextFile();

};

#endif
