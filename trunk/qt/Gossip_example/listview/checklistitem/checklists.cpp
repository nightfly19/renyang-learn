#include "checklists.h"
#include <qiconset.h>

ListView::ListView()
        : close("folder.xpm"), open("folder_open.xpm") {
    QCheckListItem *chklistitem1, *chklistitem2;

    setCaption("QListView");
    addColumn(QIconSet(close), "Folders");
    addColumn("Type");
    setColumnWidth(0, 230);

    folder = new QListViewItem(this, " parent folder", "QListViewItem");
    folder->setPixmap(0, open);
    folder->setOpen(true);

    subfolder = new QListViewItem(folder, " subfolder", "QListViewItem");
    subfolder->setPixmap(0, close);
    subfolderOn = false;

    chklistitem1 = new QCheckListItem(folder,
                       "click me", QCheckListItem::CheckBox);
    chklistitem1->setText(1, "Check Box");

    chklistitem1 = new QCheckListItem(folder, "child");
    chklistitem1->setText(1, "Controller");
    chklistitem1->setOpen(true);

    chklistitem2 = new QCheckListItem(chklistitem1,
                       "grand child 1", QCheckListItem::RadioButton);
    chklistitem2->setText(1, "Radio Button");
    chklistitem2 = new QCheckListItem(chklistitem1,
                       "grand child 2", QCheckListItem::RadioButton);
    chklistitem2->setText(1, "Radio Button");
   
    setTreeStepSize(30); // 與父節點的距離，default是20pixel
    setSorting(0, false);
    sort();

    connect(this, SIGNAL(clicked(QListViewItem*)),
            this, SLOT(clicked1(QListViewItem*)));
    connect(this, SIGNAL(doubleClicked(QListViewItem*)),
            this, SLOT(clicked2(QListViewItem*)));
}

void ListView::clicked1(QListViewItem *item) {
    if(item == subfolder) {
        subfolderOn = !subfolderOn;
        item->setPixmap(0, subfolderOn ? open : close );
    }
}

void ListView::clicked2(QListViewItem *item) {
    if(item == folder)
        item->setPixmap(0, item->isOpen() ? open : close);
}
