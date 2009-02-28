// Implementation of the Socket class
// 這一個類別主要是把c的型態轉換成c++的型態


#include "Socket.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>



Socket::Socket() :
  socketfd ( -1 ),connfd(-1)
{

  // 建立一個新的address資料結構
  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

Socket::~Socket()
{
  if ( is_valid() )
    ::close ( socketfd );
}

bool Socket::close_connfd()
{
	int return_value;
	if (is_connect_valid())
		return_value=::close(connfd);
	if (return_value == 0)
	{
		connfd = -1;
		return true;
	}
	return false;
}

bool Socket::create(int family,int type,int protocol)
{
  socketfd = ::socket (	family,
			type,
		    	protocol );
  // debug
  // printf("socket:%d\n",socketfd);
  // debug

  if ( ! is_valid() )
    return false;

  return true;

}



bool Socket::bind (const int port,int family)
{

  if ( ! is_valid() )
    {
      return false;
    }



  m_addr.sin_family = family;
  m_addr.sin_addr.s_addr = INADDR_ANY;
  m_addr.sin_port = htons ( port );

  int bind_return = ::bind ( socketfd,
			     ( struct sockaddr * ) &m_addr,
			     sizeof ( m_addr ) );


  if ( bind_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::listen(int maxconnections) const
{
  if ( ! is_valid() )
    {
      return false;
    }

  int listen_return = ::listen ( socketfd, maxconnections );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::accept ()
{
  int addr_length = sizeof ( m_addr );
  connfd = ::accept ( socketfd, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( connfd <= 0 )
    return false;
  else
    return true;
}


// 透過c內建的send函數傳送資料
bool Socket::send ( const char *s,int length ) const
{
  // c_str()把string物件回傳char的指標
  // MSG_NOSIGNAL是避免當server close後, client還透過send傳送封包
  // 當第二次send時,才會出現錯誤, 透過MSG_NOSIGNAL第一次就會回送錯誤訊息
  int status = ::send ( connfd, s, length, MSG_NOSIGNAL );
  printf("send:%d\n",status); // deubg
  if ( status == -1 )
    {
      return false;
    }
  else
    {
      return true;
    }
}


// 接收資料
// 在使用此函數接收資料,要送過來buffer的指標,大小為MAXRECV
int Socket::recv ( char *buf ) const
{
  memset ( buf, 0, MAXRECV);

  int status = ::recv ( connfd, buf, MAXRECV, 0 );
  printf("recv:%d\n",status); // debug
  if ( status == -1 )
    {
      std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
      return 0;
    }
  else if ( status == 0 )
    {
      return 0;
    }
  else
    {
      return status;
    }
}



bool Socket::connect ( const char* host, const int port ,int family)
{
  if ( ! is_valid() ) return false;

  m_addr.sin_family = family;
  m_addr.sin_port = htons ( port );

  // 將點分十進位串轉換成網路位元組序二進位值
  int status = inet_pton ( family, host, &m_addr.sin_addr );

  if ( errno == EAFNOSUPPORT ) return false;

  // 連線到m_addr(server)
  status = ::connect ( socketfd, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
  connfd = socketfd;

  if ( status == 0 )
    return true;
  else
    return false;
}

