#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <qsocket.h>

class QDate;
class QTime;

class ClientSocket : public QSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject *parent = 0, const char *name = 0);

private slots:
    void readClient();

private:
    void generateRandomTrip(const QString &from, const QString &to,
                            const QDate &date, const QTime &time);

    Q_UINT16 blockSize;
};

#endif
