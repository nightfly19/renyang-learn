// Implementation of the Socket class
// 這一個類別主要是把c的型態轉換成c++的型態


#include "Socket.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>



Socket::Socket() :
  m_sock ( -1 )
{

  // 建立一個新的address資料結構
  memset ( &m_addr,
	   0,
	   sizeof ( m_addr ) );

}

Socket::~Socket()
{
  if ( is_valid() )
    ::close ( m_sock );
}

bool Socket::create(int family,int type,int protocol)
{
  m_sock = ::socket (	family,
			type,
		    	protocol );

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

  int bind_return = ::bind ( m_sock,
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

  int listen_return = ::listen ( m_sock, maxconnections );


  if ( listen_return == -1 )
    {
      return false;
    }

  return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
  int addr_length = sizeof ( m_addr );
  new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );

  if ( new_socket.m_sock <= 0 )
    return false;
  else
    return true;
}


// 透過c內建的send函數傳送資料
bool Socket::send ( const char *s ) const
{
  // c_str()把string物件回傳char的指標
  // MSG_NOSIGNAL是避免當server close後, client還透過send傳送封包
  // 當第二次send時,才會出現錯誤, 透過MSG_NOSIGNAL第一次就會回送錯誤訊息
  int status = ::send ( m_sock, s, strlen(s), MSG_NOSIGNAL );
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

  int status = ::recv ( m_sock, buf, MAXRECV, 0 );

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
  status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );

  if ( status == 0 )
    return true;
  else
    return false;
}

