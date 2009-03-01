// Definition of the Socket class
// 宣告一個類別,記錄所有的socket會用到的函數

#ifndef Socket_class
#define Socket_class


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>


const int MAXRECV = 1024;

class Socket
{
 public:
  Socket();
  virtual ~Socket();

  // Server initialization
  bool create(int family,int type,int protocol);
  bool bind ( const int port,int family );
  bool listen(int maxconnections) const;
  bool accept ();
  bool close_connfd();

  // Client initialization
  bool connect ( const char* host, const int port,int family );

  // Data Transimission
  bool send ( const char *s,int length ) const;
  int recv ( char *buf ) const;


  void set_non_blocking ( const bool );
  bool is_valid() const { return socketfd != -1; }
  bool is_connect_valid() const { return connfd !=-1;}
  int get_connfd() { return connfd; }

 private:

  int socketfd;
  int connfd;
  sockaddr_in m_addr;

};


#endif
