#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Find"));

    label = new QLabel(tr("Find &what:"), this);
    lineEdit = new QLineEdit(this);
    label->setBuddy(lineEdit);

    caseCheckBox = new QCheckBox(tr("Match &case"), this);
    backwardCheckBox = new QCheckBox(tr("Search &backward"), this);

    findButton = new QPushButton(tr("&Find"), this);
    findButton->setDefault(true);
    findButton->setEnabled(false);

    closeButton = new QPushButton(tr("Close"), this);

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(enableFindButton(const QString &)));
    connect(findButton, SIGNAL(clicked()),
            this, SLOT(findClicked()));
    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(close()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(label);
    topLeftLayout->addWidget(lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(caseCheckBox);
    leftLayout->addWidget(backwardCheckBox);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(findButton);
    rightLayout->addWidget(closeButton);
    rightLayout->addStretch(1);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
}

void FindDialog::findClicked()
{
    QString text = lineEdit->text();
    bool caseSensitive = caseCheckBox->isOn();

    if (backwardCheckBox->isOn())
        emit findPrev(text, caseSensitive);
    else
        emit findNext(text, caseSensitive);
}

void FindDialog::enableFindButton(const QString &text)
{
    findButton->setEnabled(!text.isEmpty());
}
