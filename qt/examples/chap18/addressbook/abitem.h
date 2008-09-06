#ifndef ABITEM_H
#define ABITEM_H

#include <qlistview.h>
#include <qobject.h>

class ABItem : public QObject, public QListViewItem
{
    Q_OBJECT
    Q_PROPERTY(QString contact READ contact WRITE setContact)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString phoneNumber READ phoneNumber
               WRITE setPhoneNumber)
public:
    ABItem(QListView *listView);

    void setContact(const QString &contact);
    QString contact() const { return text(0); }
    void setAddress(const QString &address);
    QString address() const { return text(1); }
    void setPhoneNumber(const QString &number);
    QString phoneNumber() const { return text(2); }

public slots:
    void remove();
};

#endif
