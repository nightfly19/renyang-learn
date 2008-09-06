#ifndef CONNECTION_H
#define CONNECTION_H

#include <qsqldatabase.h>

inline bool createOneConnection(const QString &name)
{
    QSqlDatabase *db;
    if (name.isEmpty())
        db = QSqlDatabase::addDatabase("QSQLITEX");
    else
        db = QSqlDatabase::addDatabase("QSQLITEX", name);
    db->setDatabaseName("cdcollection.dat");
    if (!db->open()) {
        db->lastError().showMessage();
        return false;
    }
    return true;
}

inline bool createConnections()
{
    return createOneConnection("")
            && createOneConnection("ARTIST")
            && createOneConnection("CD");
}

#endif
