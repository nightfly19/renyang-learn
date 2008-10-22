#include <qdatastream.h>
#include <qdatetime.h>

#include <cstdlib>
using namespace std;

#include "clientsocket.h"

ClientSocket::ClientSocket(QObject *parent, const char *name)
    : QSocket(parent, name)
{
    connect(this, SIGNAL(readyRead()),
            this, SLOT(readClient()));
    connect(this, SIGNAL(connectionClosed()),
            this, SLOT(deleteLater()));
    connect(this, SIGNAL(delayedCloseFinished()),
            this, SLOT(deleteLater()));

    blockSize = 0;
}

void ClientSocket::readClient()
{
    QDataStream in(this);
    in.setVersion(5);

    if (blockSize == 0) {
        if (bytesAvailable() < sizeof(Q_UINT16))
            return;
        in >> blockSize;
    }

    if (bytesAvailable() < blockSize)
        return;

    Q_UINT8 requestType;
    QString from;
    QString to;
    QDate date;
    QTime time;
    Q_UINT8 flag;

    in >> requestType;
    if (requestType == 'S') {
        in >> from >> to >> date >> time >> flag;

        srand(time.hour() * 60 + time.minute());
        int numTrips = rand() % 8;
        for (int i = 0; i < numTrips; ++i)
            generateRandomTrip(from, to, date, time);

        QDataStream out(this);
        out << (Q_UINT16)0xFFFF;
    }
    close();
    if (state() == Idle)
        deleteLater();
}

void ClientSocket::generateRandomTrip(const QString &,
        const QString &, const QDate &date, const QTime &time)
{
    QByteArray block;
    QDataStream out(block, IO_WriteOnly);
    out.setVersion(5);
    Q_UINT16 duration = rand() % 200;
    out << (Q_UINT16)0 << date << time << duration
        << (Q_UINT8)1 << QString("InterCity");
    out.device()->at(0);
    out << (Q_UINT16)(block.size() - sizeof(Q_UINT16));
    writeBlock(block.data(), block.size());
}
