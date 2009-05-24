

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
#include <string.h>

#include "Error.h"

#include "SctpSocketHandler.h"

/* static functions */
int SctpSocketHandler::SctpSetMaxStream(int sd , int num = 5)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpSetMaxStream(%1,%2)").arg(sd).arg(num));
#endif
	int ret;

	struct sctp_initmsg initmsg;

	memset( &initmsg, 0, sizeof(initmsg) );
	initmsg.sinit_num_ostreams = num; 
	initmsg.sinit_max_instreams = num;
	initmsg.sinit_max_attempts = num - 1;
	ret = setsockopt( sd , IPPROTO_SCTP, SCTP_INITMSG,
			&initmsg, sizeof(initmsg) );
	return ret;
}

int SctpSocketHandler::SctpGetAssocID(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpGetAssocID(%1)").arg(sd));
#endif
	struct sctp_status status;
	int ret,in;

	in = sizeof(status);
	ret = getsockopt( sd , SOL_SCTP, SCTP_STATUS,
			(void *)&status, (socklen_t *)&in );

	return status.sstat_assoc_id; 
}

/*
int SctpSocketHandler::SctpGetState(int sd)
{
	struct sctp_status status;
	int ret,in;

	in = sizeof(status);
	ret = getsockopt( sd , SOL_SCTP, SCTP_STATUS,
			(void *)&status, (socklen_t *)&in );

	return status.sstat_state;

}*/

int SctpSocketHandler::SctpEnable(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpEnable(%1)").arg(sd));
#endif
	 struct sctp_event_subscribe events;
	int ret;

	/* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
	memset( (void *)&events, 0, sizeof(events) );
	events.sctp_data_io_event = 1;
	ret = setsockopt( sd , SOL_SCTP, SCTP_EVENTS,
			(const void *)&events, sizeof(events) );

	if( ret < 0 )
		throw Error("SctpSocketHandler::SctpEnable Error");

	return ret;
}

/* static functions */
int SctpSocketHandler::SctpSetNoDelay(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpSetNoDelay(%1)").arg(sd));
#endif
	int ret; 
	int flag = 1;
	ret = setsockopt( sd , IPPROTO_SCTP, SCTP_NODELAY,
			&flag, sizeof(flag) );
	return ret;
}


int SctpSocketHandler::SctpGetRtoMax(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpGetRtoMax(%1)").arg(sd));
#endif
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	if( ret < 0 )
		throw Error("SctpSocketHandler::SctpGetRtoMax Error");

	return rto_info . srto_max; 
}

int SctpSocketHandler::SctpGetRtoMin(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpGetRtoMin(%1)").arg(sd));
#endif
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	if( ret < 0 )
		throw Error("SctpSocketHandler::SctpGetRtoMin Error");

	return rto_info . srto_min; 
}

int SctpSocketHandler::SctpSetRtoMax(int sd , int n)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpSetRtoMax(%1,%2)").arg(sd).arg(n));
#endif
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, (socklen_t *)&in );

	rto_info . srto_max = n; 
	ret = setsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, in );

	return ret;
}

int SctpSocketHandler::SctpSetRtoMin(int sd , int n)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpSetRtoMin(%1,%2)").arg(sd).arg(n));
#endif
	struct sctp_rtoinfo rto_info;
	int ret,in;

	in = sizeof(rto_info);
	ret = getsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info,(socklen_t *)& in );

	rto_info . srto_min = n; 
	ret = setsockopt( sd , SOL_SCTP, SCTP_RTOINFO,
			(void *)&rto_info, in );

	return ret; 
}


int SctpSocketHandler::SctpSetAutoClose(int sd , int n)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpSetAutoClose(%1,%2)").arg(sd).arg(n));
#endif
	int ret;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_AUTOCLOSE, &n, sizeof (n)); 

	return ret; 
}

int SctpSocketHandler::SctpTurnOnAllEvent(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpTurnOnAllEvent(%1)").arg(sd));
#endif
	int ret;
	struct sctp_event_subscribe event;
	int n = sizeof(event);

	event . sctp_data_io_event = 1;
	event . sctp_association_event = 1;
	event . sctp_address_event = 1;
	event . sctp_send_failure_event = 1;
	event . sctp_peer_error_event = 1;
	event . sctp_shutdown_event = 1;
	event . sctp_partial_delivery_event = 1;
	event . sctp_adaptation_layer_event = 1;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &event , n); 

	return ret; 
}

int SctpSocketHandler::SctpTurnOffAllEvent(int sd)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpTurnOffAllEvent(%1)").arg(sd));
#endif
	int ret;
	struct sctp_event_subscribe event;
	int n = sizeof(event);

	event .  sctp_data_io_event = 0;
	event . sctp_association_event = 0;
	event . sctp_address_event = 0;
	event . sctp_send_failure_event = 0;
	event . sctp_peer_error_event = 0;
	event . sctp_shutdown_event = 0;
	event . sctp_partial_delivery_event = 0;
	event . sctp_adaptation_layer_event = 0;

	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &event , n); 

	return ret; 
}

QString SctpSocketHandler::SctpGetPrim(int sd , int assoc_id)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler::SctpGetPrim(%1,%2)").arg(sd).arg(assoc_id));
#endif
	struct sockaddr_in *in_addr;
	struct sockaddr_in6 *in6_addr;
	char addr_buf[INET6_ADDRSTRLEN];
	struct sockaddr *saddr = NULL;
	struct sctp_prim prim;
	socklen_t prim_len= sizeof(prim);
	int ret;
	const char *ap = NULL;

	prim.ssp_assoc_id = assoc_id;
	ret = getsockopt(sd, IPPROTO_SCTP, SCTP_PRIMARY_ADDR, &prim, (socklen_t*)&prim_len);
	if (ret < 0){
		debug("1");
		throw Error("Error: SctpGetPrim ");
	} 

	saddr = (struct sockaddr *)&prim.ssp_addr;
	if (AF_INET == saddr->sa_family) {
		in_addr = (struct sockaddr_in *)&prim.ssp_addr;
		ap = inet_ntop(AF_INET, &in_addr->sin_addr, addr_buf,
				INET6_ADDRSTRLEN);
	} else if (AF_INET6 == saddr->sa_family) {
		in6_addr = (struct sockaddr_in6 *)&prim.ssp_addr;
		ap = inet_ntop(AF_INET6, &in6_addr->sin6_addr, addr_buf,
				INET6_ADDRSTRLEN);
	}
	if (!ap){
		debug("1");
		throw Error("Error: SctpGetPrim ");
	}

	debug("get prim %s",ap);
	QString ret_str(ap);

	return ret_str;
}

/*
void SctpSocketHandler :: SctpSetPrim(int sd , int associd , int port , QString str)
{
	struct sctp_setpeerprim setpeerprim;
	int peer_prim_len, ret;

	struct hostent *hostInfo = NULL;
	struct sockaddr_in *in_addr;
	struct sockaddr_in6 *in6_addr;

	port = 0;

	peer_prim_len = sizeof(struct sctp_setpeerprim);

	debug(" SctpSetPri(%d,%d,%d,"+str+")",sd,associd,port);

	setpeerprim.sspp_assoc_id = associd;	
	if (1) { // not support v6 yet
		in_addr = (struct sockaddr_in *)&setpeerprim.sspp_addr;

		in_addr->sin_port = htons(port);
		in_addr->sin_family = AF_INET;

		hostInfo = ::gethostbyname( str.ascii() );
		if( !hostInfo)
			throw Error("SctpSetPrim :not valid IPv4 format" );

		ret = inet_pton (AF_INET, str.ascii() , &in_addr->sin_addr);

		if(ret < 0){
			debug("Failed to inet_pton");
		}

	} else if (false) {
		in6_addr = (struct sockaddr_in6 *)&setpeerprim.sspp_addr;
		in6_addr->sin6_port = htons(port);
		in6_addr->sin6_family = AF_INET6;
		ret = inet_pton(AF_INET6, str.ascii() , &in6_addr->sin6_addr);
	} 


	ret = setsockopt(sd, IPPROTO_SCTP, SCTP_PRIMARY_ADDR, &setpeerprim, sizeof(struct sctp_setpeerprim));

	if(ret < 0){
		debug("Failed to set primary");
	}

	//TODO: ERROR HANDLER
	return;
}*/

void SctpSocketHandler :: SctpSetPrim(int sd , int index)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler :: SctpSetPrim(%1,%2)").arg(sd).arg(index));
#endif
	int ret = 0;
	struct sockaddr *addr_list;
	//struct sockaddr_in *in;
	//struct sctp_setpeerprim setpeerprim;
	struct sctp_prim setpeerprim;


	// TODO: assoc_id ingore in one-one
	int sock_num = sctp_getpaddrs( sd , 0 , &addr_list );


	if( sock_num > 0 ){
		memcpy((char *) & setpeerprim.ssp_addr,
				& addr_list[index], sizeof(sockaddr)); 

		ret = setsockopt(sd, IPPROTO_SCTP, SCTP_PRIMARY_ADDR, &setpeerprim, sizeof(struct sctp_setpeerprim));

		if(ret < 0){
			debug("Failed to set primary");
		}

		sctp_freepaddrs( addr_list );
	}else{

	}

	//TODO: ERROR HANDLER
	return;
}

void SctpSocketHandler :: SctpSetPeerPrim(int sd , int index)
{
#ifdef REN_DEBUG
	qWarning(QString("SctpSocketHandler :: SctpSetPeerPrim(%1,%2)").arg(sd).arg(index));
#endif
	int ret = 0;
	struct sockaddr *addr_list;
	//struct sockaddr_in *in;
	struct sctp_setpeerprim setpeerprim;


	// TODO: assoc_id ingore in one-one
	int sock_num = sctp_getladdrs( sd , 0 , &addr_list );


	if( sock_num > 0 ){
		setpeerprim.sspp_assoc_id = 1;
		memcpy((char *) & setpeerprim.sspp_addr,
				& addr_list[index], sizeof(sockaddr)); 

		ret = setsockopt(sd, IPPROTO_SCTP, SCTP_SET_PEER_PRIMARY_ADDR ,
			&setpeerprim, sizeof(struct sctp_setpeerprim));

		if(ret < 0){
			debug("Failed to set peer primary");
		}

		sctp_freeladdrs( addr_list );
	}else{

	}

	//TODO: ERROR HANDLER
	return;
}
