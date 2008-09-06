#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "abitem.h"
#include "editdialog.h"

EditDialog::EditDialog(QListView *view, QWidget *parent,
                       const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Add Entry"));
    init();
    listView = view;
    currentItem = 0;
}

EditDialog::EditDialog(ABItem *item, QWidget *parent,
                       const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Edit Entry"));
    init();
    contactLineEdit->setText(item->contact());
    addressLineEdit->setText(item->address());
    phoneLineEdit->setText(item->phoneNumber());
    listView = item->listView();
    currentItem = item;
}

void EditDialog::init()
{
    contactLabel = new QLabel(tr("&Contact:"), this);
    addressLabel = new QLabel(tr("&Address:"), this);
    phoneLabel = new QLabel(tr("&Phone Number:"), this);
    contactLineEdit = new QLineEdit(this);
    addressLineEdit = new QLineEdit(this);
    phoneLineEdit = new QLineEdit(this);

    contactLabel->setBuddy(contactLineEdit);
    addressLabel->setBuddy(addressLineEdit);
    phoneLabel->setBuddy(phoneLineEdit);

    okButton = new QPushButton(tr("OK"), this);
    okButton->setDefault(true);
    cancelButton = new QPushButton(tr("Cancel"), this);

    connect(okButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(contactLabel, 0, 0);
    mainLayout->addWidget(contactLineEdit, 0, 1);
    mainLayout->addWidget(addressLabel, 1, 0);
    mainLayout->addWidget(addressLineEdit, 1, 1);
    mainLayout->addWidget(phoneLabel, 2, 0);
    mainLayout->addWidget(phoneLineEdit, 2, 1);
    mainLayout->addMultiCellLayout(buttonLayout, 3, 3, 0, 1);

    resize(350, sizeHint().height());
}

void EditDialog::accept()
{
    if (!currentItem)
        currentItem = new ABItem(listView);
    currentItem->setContact(contactLineEdit->text());
    currentItem->setAddress(addressLineEdit->text());
    currentItem->setPhoneNumber(phoneLineEdit->text());
    QDialog::accept();
}
