#ifndef THREADFORM_H
#define THREADFORM_H

#include <qdialog.h>

#include "thread.h"

class QPushButton;

class ThreadForm : public QDialog
{
    Q_OBJECT
public:
    ThreadForm(QWidget *parent = 0, const char *name = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void startOrStopThreadA();
    void startOrStopThreadB();

private:
    Thread threadA;
    Thread threadB;
    QPushButton *threadAButton;
    QPushButton *threadBButton;
    QPushButton *quitButton;
};

#endif
