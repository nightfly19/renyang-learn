

/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */
#ifndef __SCTP_SOCKET_HANDLER_H__
#define __SCTP_SOCKET_HANDLER_H__

#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>

class SctpSocketHandler
{
	public:
		enum SCTP_EVENT_ITEM {
			SCTP_IO_EVENT,
			SCTP_ASSOCIATION_EVENT,
			SCTP_ADDRESS_EVENT,
			SCTP_SEND_FAILUR_EVENT,
			SCTP_PEER_ERROR_EVENT,
			SCTP_SHUTDOWN_EVENT,
			SCTP_PARTIAL_DELIVERY_EVENT,
			SCTP_ADAPTION_LAYER_EVENT,
		};
		

	public:
		/* Static Sctp extention */ 
		static int SctpSetMaxStream(int sd,int num);
		static int SctpSetNoDelay(int sd);

		static int SctpGetAssocID(int sd);
		static int SctpEnable(int sd);

		static int SctpGetRtoMax(int sd);
		static int SctpGetRtoMin(int sd);
		static int SctpSetRtoMax(int sd, int);
		static int SctpSetRtoMin(int sd, int);

		static int SctpSetAutoClose(int sd, int);

		/*static int SctpSetEventNotify(int sd, SCTP_EVENT_ITEM);
		static int SctpGetEventNotify(int sd, SCTP_EVENT_ITEM);
		*/

		static int SctpTurnOnAllEvent(int sd);
		static int SctpTurnOffAllEvent(int sd);
		
		static QString SctpGetPrim(int sd , int assoc_id);
		static void SctpSetPrim(int sd , int index);
		static void SctpSetPeerPrim(int sd , int index);

	private:

}; 

#endif
