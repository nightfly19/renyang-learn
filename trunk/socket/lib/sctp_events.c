#include "unp.h"

void SctpTurnOnAllEvent(int sockfd)
{
	struct sctp_event_subscribe event;
	bzero(&event,sizeof(event));
	
	event.sctp_data_io_event = 1;
	event.sctp_association_event = 1;
	event.sctp_address_event = 1;
	event.sctp_send_failure_event = 1;
	event.sctp_peer_error_event = 1;
	event.sctp_shutdown_event = 1;
	event.sctp_partial_delivery_event = 1;
	event.sctp_adaptation_layer_event = 1;

	Setsockopt(sockfd,IPPROTO_SCTP,SCTP_EVENTS,&event,sizeof(event));
}

void SctpTurnOffAllEvent(int sockfd)
{
	struct sctp_event_subscribe event;
	bzero(&event,sizeof(event));

	Setsockopt(sockfd,IPPROTO_SCTP,SCTP_EVENTS,&event,sizeof(event));
}
