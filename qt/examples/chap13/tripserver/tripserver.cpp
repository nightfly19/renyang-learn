#include "clientsocket.h"
#include "tripserver.h"

TripServer::TripServer(QObject *parent, const char *name)
    : QServerSocket(6178, 1, parent, name)
{
}

void TripServer::newConnection(int socketId)
{
    ClientSocket *socket = new ClientSocket(this);
    socket->setSocket(socketId);
}
