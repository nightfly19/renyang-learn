#include "abfactory.h"
#include "addressbook.h"

ABFactory::ABFactory(const QUuid &lib, const QUuid &app)
    : QAxFactory(lib, app)
{
}

QStringList ABFactory::featureList() const
{
    return QStringList() << "AddressBook" << "ABItem";
}

QWidget *ABFactory::create(const QString &key, QWidget *parent,
                           const char *name)
{
    if (key == "AddressBook")
        return new AddressBook(parent, name);
    else
        return 0;
}

QUuid ABFactory::classID(const QString &key) const
{
    if (key == "AddressBook")
        return QUuid("{588141ef-110d-4beb-95ab-ee6a478b576d}");
    else if (key == "ABItem")
        return QUuid("{bc82730e-5f39-4e5c-96be-461c2cd0d282}");
    else
        return QUuid();
}

QUuid ABFactory::interfaceID(const QString &key) const
{
    if (key == "AddressBook")
        return QUuid("{718780ec-b30c-4d88-83b3-79b3d9e78502}");
    else if (key == "ABItem")
        return QUuid("{c8bc1656-870e-48a9-9937-fbe1ceff8b2e}");
    else
        return QUuid();
}

QUuid ABFactory::eventsID(const QString &key) const
{
    if (key == "AddressBook")
        return QUuid("{0a06546f-9f02-4f14-a269-d6d56ffeb861}");
    else if (key == "ABItem")
        return QUuid("{105c6b0a-3fc7-460b-ae59-746d9d4b1724}");
    else
        return QUuid();
}

QString ABFactory::exposeToSuperClass(const QString &key) const
{
    return key;
}
