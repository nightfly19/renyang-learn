#ifndef QCHECK_LIST
#define QCHECK_LIST

#include <qlistview.h>
#include <qpixmap.h>

class ListView : public QListView {
    Q_OBJECT

public:
    ListView();

protected:
    QListViewItem *folder, *subfolder;
    QPixmap close, open;
    bool subfolderOn;

protected slots:
    void clicked1(QListViewItem*);
    void clicked2(QListViewItem*);
};

#endif
