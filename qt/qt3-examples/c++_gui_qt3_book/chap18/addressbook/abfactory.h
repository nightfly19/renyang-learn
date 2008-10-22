#ifndef ABFACTORY_H
#define ABFACTORY_H

#include <qaxfactory.h>

class ABFactory : public QAxFactory
{
public:
    ABFactory(const QUuid &lib, const QUuid &app);
    QStringList featureList() const;
    QWidget *create(const QString &key, QWidget *parent,
                    const char *name);
    QUuid classID(const QString &key) const;
    QUuid interfaceID(const QString &key) const;
    QUuid eventsID(const QString &key) const;
    QString exposeToSuperClass(const QString &key) const;
};

#endif
