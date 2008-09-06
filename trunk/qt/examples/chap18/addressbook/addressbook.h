#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <qmainwindow.h>

class QAction;
class QListView;
class QPopupMenu;
class ABItem;

class AddressBook : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
public:
    AddressBook(QWidget *parent = 0, const char *name = 0);
    ~AddressBook();

    int count() const;

public slots:
    ABItem *createEntry(const QString &contact);
    ABItem *findEntry(const QString &contact) const;
    ABItem *entryAt(int index) const;

private slots:
    void addEntry();
    void editEntry();
    void deleteEntry();

private:
    void createActions();
    void createMenus();

    QListView *listView;

    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QAction *exitAct;
    QAction *addEntryAct;
    QAction *editEntryAct;
    QAction *deleteEntryAct;
};

#endif
