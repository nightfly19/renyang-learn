#ifndef STDINFO_H
#define STDINFO_H

#define LOGIN		"login"
#define ADDRESS		"address"
#define TIME		"time"
#define DATE		"date"

/**
 * General message.
 */
#define MESSAGE		"Message"

#define VG_STATE	"VGState"
#define CALL_STATE	"CallState"
#define GK_STATE	"GkState"

enum VG_StateType {
	VG_IDLE,
	VG_OPENING,
	VG_OPENED,
	VG_CLOSED
};

enum CALL_StateType {
	CALL_IDLE,
	CALL_STARTED,
	CALL_CLOSED,
	CALL_CALLING,
	CALL_HELD
};

enum GK_StateType {
	/** 
	 * Registration with gatekeeper have not occured yet.
	 */
	GK_IDLE,

	/** 
	 * Registration with gatekeeper is in progress.
	 */
	GK_REGISTERING,

	/**
	 * Application is registered with gatekeeper.
	 */
	GK_REGISTERED,

	/**
	 * Registration with gatekeeper was unsuccessful.
	 */
	GK_NOT_REGISTERED
};

#define JITTER_BUFFER_SIZE	"jitterBufferSize"
#define JITTER_AVERAGE_TIME	"jitterAverageTime"
#define JITTER_MAXIMUM_TIME	"jitterMaximumTime"
#define TRANSMITTER_SESSION_INTERVAL	"transmitterSessionInterval"
#define RECEIVER_SESSION_INTERVAL	"receiverSessionInterval"
#define PACKETS_SENT		"packetsSent"
#define PACKETS_RECEIVED	"packetsReceived"
#define PACKETS_LOST		"packetsLost"
#define PACKETS_TOO_LATE	"packetsTooLate"

#define PARTY_NAME		"partyName"
#define PARTY_HOST		"partyHost"
#define PARTY_PICTURE		"partyPicture"
#define CALL_DURATION		"callDuration"

#define ADDRESS_TO_CALL		"addressToCall"
#define ADDRESS_TO_CALL_NOW	"addressToCallNow"
#define ADDRESS_CALLING		"addressCalling"
#define CALLER_ADDRESS		"callerAddress"

#define HTTP_PUSH		"httpPush"
#define HTTP_CLOSE		"httpClose"

#endif
