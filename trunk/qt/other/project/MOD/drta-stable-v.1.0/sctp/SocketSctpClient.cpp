

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netdb.h>

#include "Error.h"
#include "SocketSctpClient.h"
#include "SocketClient.h"
#include "global.h"


SocketSctpClient::SocketSctpClient()
	: SocketClient(SOCK_STREAM , IPPROTO_SCTP)
	, _port(0)
{
	SctpEnable();

	_connx_ip_list . setAutoDelete( true );
}

SocketSctpClient::~SocketSctpClient()
{
	_connx_ip_list . clear();
}

int SocketSctpClient::SctpSetMaxStream(int num = 5)
{
	int ret;

	struct sctp_initmsg initmsg;

	memset( &initmsg, 0, sizeof(initmsg) );
	initmsg.sinit_num_ostreams = num; 
	initmsg.sinit_max_instreams = num;
	initmsg.sinit_max_attempts = num - 1;
	ret = setsockopt( _socket, IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof(initmsg) );
	return ret;
}

int SocketSctpClient::SctpGetAssocID()
{
	struct sctp_status status;
	int ret,in;

	in = sizeof(status);
	ret = getsockopt( _socket, SOL_SCTP, SCTP_STATUS,
			(void *)&status, (socklen_t *)&in );

	return status.sstat_assoc_id; 
}
int SocketSctpClient::SctpGetState()
{
	struct sctp_status status;
	int ret,in;

	in = sizeof(status);
	ret = getsockopt( _socket, SOL_SCTP, SCTP_STATUS,
			(void *)&status, (socklen_t *)&in );

	return status.sstat_state;

}

int SocketSctpClient::SctpEnable()
{
	 struct sctp_event_subscribe events;
	int ret;

	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset( (void *)&events, 0, sizeof(events) );
	events.sctp_data_io_event = 1;
	ret = setsockopt( _socket , SOL_SCTP, SCTP_EVENTS,
			(const void *)&events, sizeof(events) );

	return ret;
}
int SocketSctpClient::SctpSendMsg(mPacket& p,int str_num, int sock)
{
	int ret=0;

	if(sock  == -1) sock=_socket;
			
	ret = sctp_sendmsg(sock, p.getBufPtr() , p.Size() 
			, NULL , 0 , 0 , 0 , str_num , 0 , 0);
	if(ret < 0)
		throw Error("SocketSctpClient::send error");
	return ret;
}

int SocketSctpClient::SctpRecvMsg(mPacket& p,int& str_num, int sock)
{
	int ret=0;
	struct sctp_sndrcvinfo sndrcvinfo;

	if(sock  == -1) sock=_socket;
			
	ret = sctp_recvmsg(sock, p.getBufPtr() , p.MaxSize() 
			, NULL , 0 , &sndrcvinfo , 0);

	if(ret < 0)
		throw Error("SocketSctpClient::send error");

	str_num = sndrcvinfo.sinfo_stream;
	return ret;
}


int SocketSctpClient::SctpAddConn(QString& ip , int port)
{
	struct hostent *hostInfo = NULL;
	//struct sockaddr_in *server_adr = NULL;

	hostInfo = gethostbyname(ip . latin1());
	if(!hostInfo)	return 0; // error 

	/*
	server_adr = new sockaddr_in();
	server_adr -> sin_family = hostInfo->h_addrtype;
	server_adr -> sin_port = htons(port);
	memcpy((char *) &server_adr -> sin_addr.s_addr,
			hostInfo->h_addr_list[0], hostInfo->h_length); 
	*/

	char* buf = new char[ip . length() + 1];
	memcpy((char *) buf , ip . latin1() , ip . length() + 1); 
	buf[ ip . length() ] = '\0';

	_connx_ip_list . append( buf );
	_port = port;


	return 1;
}

int SocketSctpClient::SctpConnect()
{
	struct hostent *hostInfo = NULL;
	int ret = 0;

	int adr_size = _connx_ip_list . count();

	struct sockaddr_in *ip_list= new sockaddr_in[adr_size];

	for(int i = 0 ; i < adr_size ; i++){
		debug("SctpConnectx -  ip %d - %s",i , _connx_ip_list.at(i));

		hostInfo = gethostbyname( _connx_ip_list.at(i) );

		ip_list[i] . sin_family = hostInfo->h_addrtype;
		ip_list[i] . sin_port = htons(_port);
		memcpy((char *) &ip_list[i] . sin_addr.s_addr,
				hostInfo->h_addr_list[0], hostInfo->h_length); 
	}
	
	ret = sctp_connectx(_socket , (struct sockaddr*) ip_list , adr_size );
	if( ret < 0){
		debug("connnection refuse?"); 
	}
	debug("SctpConnect ret =%d",ret);

	SocketClient::setSocket(_socket);

	delete ip_list;

	return ret;
}
