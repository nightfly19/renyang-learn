#include <qlayout.h>
#include <qpushbutton.h>

#include "threadform.h"

ThreadForm::ThreadForm(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Threads"));

    threadA.setMessage("A");
    threadB.setMessage("B");

    threadAButton = new QPushButton(tr("Start A"), this);
    threadBButton = new QPushButton(tr("Start B"), this);
    quitButton = new QPushButton(tr("Quit"), this);
    quitButton->setDefault(true);

    connect(threadAButton, SIGNAL(clicked()),
            this, SLOT(startOrStopThreadA()));
    connect(threadBButton, SIGNAL(clicked()),
            this, SLOT(startOrStopThreadB()));
    connect(quitButton, SIGNAL(clicked()),
            this, SLOT(close()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setResizeMode(QLayout::Fixed);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(threadAButton);
    mainLayout->addWidget(threadBButton);
    mainLayout->addWidget(quitButton);
}

void ThreadForm::startOrStopThreadA()
{
    if (threadA.running()) {
        threadA.stop();
        threadAButton->setText(tr("Start A"));
    } else {
        threadA.start();
        threadAButton->setText(tr("Stop A"));
    }
}

void ThreadForm::startOrStopThreadB()
{
    if (threadB.running()) {
        threadB.stop();
        threadBButton->setText(tr("Start B"));
    } else {
        threadB.start();
        threadBButton->setText(tr("Stop B"));
    }
}

void ThreadForm::closeEvent(QCloseEvent *event)
{
    threadA.stop();
    threadB.stop();
    threadA.wait();
    threadB.wait();
    event->accept();
}
