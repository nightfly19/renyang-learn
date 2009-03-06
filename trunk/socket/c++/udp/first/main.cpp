#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <iostream>
using namespace std;

enum { RECV_PORT = 5000, MSGSIZE = 1024 };
const char fileName[30] = "file.txt";
char* s;
char input[255];
char* result = NULL;
string qns, ans;
socklen_t addr_len = sizeof(sockaddr) ;

int server( int socket_fd )
{
    sockaddr_in my_addr ;
    memset( &my_addr, 0, sizeof(my_addr) ) ;
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons( RECV_PORT ) ;
    my_addr.sin_addr.s_addr = INADDR_ANY ;

    if ( bind( socket_fd, (sockaddr*)&my_addr, addr_len ) != 0 )
       return 2 ;
	
    ifstream infile(fileName);
    while( true )
    {
	//infile.open(fileName);
			
        char recv_data[MSGSIZE+1] ;
        sockaddr_in client_addr ;

        int bytes_recd = recvfrom( socket_fd, recv_data, MSGSIZE, 0,
                                (sockaddr*)&client_addr, &addr_len ) ;
        if( bytes_recd == -1 ) 
	break ;
	
	else
	
	
	recv_data[bytes_recd] = '\0' ;
	
	{
	
	infile.getline(input,255, '\n');
				
	vector<string> parts;
	string tmpstr(input);
	s = input;	

	result = strtok(s, ":");

	while (result != NULL)  
	{
		if (result != "" && result != ":") 
		{
			parts.push_back(result);
		}

         	result = strtok(NULL, ":");
     	}
		
	qns = parts[0];
	ans = parts[1];
	
	}
	string tmpvar(recv_data);

        cout << "from " << inet_ntoa(client_addr.sin_addr)
                  << ':' << ntohs(client_addr.sin_port) << " - "
                  << recv_data << endl ;
	
	if (tmpvar == qns)
	cout << ans << endl;
	

    }
    return 0 ;
}

int client( int socket_fd )
{
    cout << "Enter Address to connect: " ;
    string address ;
    cin >> address >> ws ;

    sockaddr_in peer_addr ;
    memset( &peer_addr, 0, sizeof(peer_addr) ) ;
    peer_addr.sin_family = AF_INET ;
    peer_addr.sin_port = htons( RECV_PORT ) ;
    peer_addr.sin_addr.s_addr =
             *(in_addr_t*)(gethostbyname( address.c_str() )->h_addr) ;

    string send_str ;
    while( std::getline( std::cin, send_str ) )
    {
      send_str.resize(MSGSIZE) ;
      sendto( socket_fd, send_str.c_str(), MSGSIZE, 0,
            (sockaddr*)&peer_addr, addr_len ) ;
    }
    return 0 ;
}

int main()
{
  int socket_fd = socket( AF_INET, SOCK_DGRAM, 0 ) ;
  if( socket_fd == -1 ) return 1 ;
  return fork() == 0 ? server( socket_fd ) : client( socket_fd ) ;
}
