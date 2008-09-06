#include <qapplication.h>

#include "abfactory.h"
#include "addressbook.h"

QAXFACTORY_EXPORT(ABFactory,
                  "{2b2b6f3e-86cf-4c49-9df5-80483b47f17b}",
                  "{8e827b25-148b-4307-ba7d-23f275244818}")

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!QAxFactory::isServer()) {
        AddressBook addressBook;
        app.setMainWidget(&addressBook);
        addressBook.show();
        return app.exec();
    }
    return app.exec();
}
