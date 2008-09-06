#include <qaction.h>
#include <qdir.h>
#include <qlistview.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtextstream.h>

#include "abitem.h"
#include "addressbook.h"
#include "editdialog.h"

AddressBook::AddressBook(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{
    setCaption(tr("Address Book"));

    listView = new QListView(this);
    listView->addColumn(tr("Name"));
    listView->addColumn(tr("Address"));
    listView->addColumn(tr("Phone Number"));
    listView->setAllColumnsShowFocus(true);
    listView->setResizeMode(QListView::LastColumn);
    setCentralWidget(listView);

    createActions();
    createMenus();

    QFile file(QDir::homeDirPath() + "/addressbook.txt");
    if (file.open(IO_ReadOnly | IO_Translate)) {
        QTextStream input(&file);
        input.setEncoding(QTextStream::UnicodeUTF8);

        while (!input.eof()) {
            ABItem *entry = new ABItem(listView);
            entry->setContact(input.readLine());
            entry->setAddress(input.readLine());
            entry->setPhoneNumber(input.readLine());
            input.readLine();
        }
    }
}

AddressBook::~AddressBook()
{
    QFile file(QDir::homeDirPath() + "/addressbook.txt");
    if (file.open(IO_WriteOnly | IO_Translate)) {
        QTextStream output(&file);
        output.setEncoding(QTextStream::UnicodeUTF8);
        QListViewItemIterator it(listView);
        while (it.current()) {
            ABItem *entry = (ABItem*)it.current();
            output << entry->contact() << endl;
            output << entry->address() << endl;
            output << entry->phoneNumber() << endl;
            output << endl;
            ++it;
        }
    }
}

void AddressBook::createActions()
{
    exitAct = new QAction(tr("E&xit"), tr("Ctrl+Q"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(activated()), this, SLOT(close()));

    addEntryAct = new QAction(tr("&Add Entry..."), tr("Ins"), this);
    addEntryAct->setStatusTip(tr("Add a new entry to the address "
                                 "book"));
    connect(addEntryAct, SIGNAL(activated()),
            this, SLOT(addEntry()));

    editEntryAct = new QAction(tr("&Edit Entry..."), tr("F2"), this);
    editEntryAct->setStatusTip(tr("Edit the current address book "
                                  "entry"));
    connect(editEntryAct, SIGNAL(activated()),
            this, SLOT(editEntry()));

    deleteEntryAct = new QAction(tr("&Delete Entry"), tr("Del"),
                                 this);
    deleteEntryAct->setStatusTip(tr("Delete the current address "
                                    "book entry"));
    connect(deleteEntryAct, SIGNAL(activated()),
            this, SLOT(deleteEntry()));
}

void AddressBook::createMenus()
{
    fileMenu = new QPopupMenu(this);
    exitAct->addTo(fileMenu);

    editMenu = new QPopupMenu(this);
    addEntryAct->addTo(editMenu);
    editEntryAct->addTo(editMenu);
    deleteEntryAct->addTo(editMenu);

    menuBar()->insertItem(tr("&File"), fileMenu);
    menuBar()->insertItem(tr("&Edit"), editMenu);
}

ABItem *AddressBook::createEntry(const QString &contact)
{
    ABItem *item = new ABItem(listView);
    item->setContact(contact);
    return item;
}

ABItem *AddressBook::findEntry(const QString &contact) const
{
    return (ABItem *)listView->findItem(contact, 0);
}

ABItem *AddressBook::entryAt(int index) const
{
    if (index > listView->childCount())
        return 0;

    QListViewItemIterator it(listView);
    it += index - 1;
    return (ABItem *)it.current();
}

int AddressBook::count() const
{
    return listView->childCount();
}

void AddressBook::addEntry()
{
    EditDialog dialog(listView, this);
    dialog.exec();
}

void AddressBook::editEntry()
{
    ABItem *item = (ABItem *)listView->currentItem();
    if (item) {
        EditDialog dialog(item, this);
        dialog.exec();
    }
}

void AddressBook::deleteEntry()
{
    delete listView->currentItem();
}
