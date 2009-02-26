// SocketException class


#ifndef SocketException_class
#define SocketException_class

#include <string>

// 建立一個例外的物件
class SocketException
{
 public:
  SocketException ( std::string s ) : m_s ( s ) {};
  ~SocketException (){};

  std::string description() { return m_s; }

 private:

  std::string m_s;	// 例外錯誤的內容

};

#endif
