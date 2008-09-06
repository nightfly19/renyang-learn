#ifndef GENERATEID_H
#define GENERATEID_H

#include <qsqldatabase.h>

inline int generateId(const QString &table, QSqlDatabase *db)
{
    QSqlQuery query(db);
    query.exec("SELECT max(id) FROM " + table);
    query.next();
    return query.value(0).toInt() + 1;
}

#endif
