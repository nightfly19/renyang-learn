#ifndef TRIPSERVER_H
#define TRIPSERVER_H

#include <qserversocket.h>

class TripServer : public QServerSocket
{
public:
    TripServer(QObject *parent = 0, const char *name = 0);

    void newConnection(int socket);
};

#endif
