#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <ptlib.h>
#include <h323.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class EndPointPrivate;
class RingThread;

/**
 * @short Basic class of OpenH323 library.
 *
 * It implements some signals of OpenH323 and contains several call 
 * oriented methods:
 * 	- initialize endpoint (#init)
 * 	- register with gatekeeper (#registerGk)
 * 	- make new call (#makeCall)
 * 	- hand up current call (#hangUpCurrent)
 */
class EndPoint: public H323EndPoint 
{
	friend class RingThread;
	PCLASSINFO(EndPoint, H323EndPoint)
public:
	EndPoint( PConfigArgs& args );
	~EndPoint();

	/**
	 * @short Initialize H323 EndPoint.
	 * @returns true if initialization was successful
	 *
	 * It sets up endpoint capabilities and calls #registerGk. It also
	 * sets some basic variables.
	 */
	bool init(); 

	/**
	 * @short Type of gatekeeper registration.
	 */
	enum RegistrationMethod {
		GkAuto, /**< Automatic gatekeeper discovery */
		GkAddr, /**< Register with gatekeeper of the specified address */
		GkId,   /**< Register with gatekeeper of the specified identifier */
		GkBoth  /**< Register with gatekeeper of the specified address and identifier */
	};

#ifdef ENTERPRISE
	/**
	 * @short Get required information from LDAP server.
	 */
	void fetchLDAP();
#endif

	/**
	 * @short Registers with gatekeeper.
	 * @returns true if registration was successful
	 */
	bool registerGk();

#ifndef NO_CALL_DIVERSIONS
	/**
	 * @short Sets server side call diversions if available.
	 *
	 * Client must be registered with gatekeeper that allows H450.3 Call Diversion.
	 * The diversion is set according to a global settings in AncaConf,
	 * particulary to USE_GK_DIVERSION and FORWARD_HOST, ALWAYS_FORWARD,
	 * BUSY_FORWARD, NO_ANSWER_FORWARD.
	 *
	 * This function should be called only by SlaveThread since it wants to be
	 * synchronous (similar to #interrogateDiversion).
	 */
	void setDiversion();

	/**
	 * @short Does H323EndPoint::InterrogateDiversion() query.
	 * 
	 * This function should be called only by SlaveThread since it wants to be
	 * synchronous (unlike H323EndPoint::InterrogateDiversion and 
	 * H323EndPoint::OnInterrogateDiversionComplete).
	 */
	void interrogateDiversion();

	/**
	 * @short Sets if InterrogateDiversion() has already been done.
	 *
	 * This function is thread safe.
	 */
	void setDiversionInterrogated( bool set );

	/**
	 * @short Has InterrogateDiversion() already been done?
	 *
	 * This function is thread safe.
	 */
	bool diversionInterrogated();
#endif

	/**
	 * @short Makes a new call
	 * @param remote address of the callee
	 * @returns false if connection could not be established (wrong remote party name etc.)
	 */
	bool makeCall( const PString& remote );
	
	/**
	 * @short Hangs up current call
	 */
	void hangUpCurrent();
	
	/**
	 * @short Holds or retrieve call
	 * @returns true if H323EndPoint::HoldCall was successful, false if not
	 * in call or RetrieveCall was successful.
	 */
	bool holdRetrieveCall();

	bool transferCall( const PString& remoteParty );

	bool transferHeldCall();

	/**
	 * @short Opens a channel for use by an audio codec when a call is going
	 * to be established
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual BOOL OpenAudioChannel(H323Connection & connection, BOOL isEncoding, unsigned bufferSize, H323AudioCodec & codec);

	/**
	 * @short Opens a channel for use by a video codec when a call is going
	 * to be established
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual BOOL OpenVideoChannel( H323Connection & connection, BOOL isEncoding, H323VideoCodec & codec );

	/**
	 * @short Setup the transfer of an existing call (connection) to a new remote
	 * party using H4502. 
	 *
	 * This sends a Call Transfer Setup Invoke message from the B-Party
	 * (transferred endpoint) to the C-Party (transferred-to endpoint).
	 * See H323EndPoint documentation for more information.
	 */
	virtual H323Connection *SetupTransfer(const PString& token, const
			PString& callIdentity, const PString& remoteParty,
			PString& newToken, void *userData = NULL);

	/**
	 * @short Signal indicating that a connection to a remote endpoint was
	 * established
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual void OnConnectionEstablished(H323Connection &connection, const PString &token);

	/**
	 * @short Signal indicating that a connection to a remote endpoint is
	 * no longer available
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual void OnConnectionCleared(H323Connection &connection, const PString &token);

	/**
	 * @short A call back function when a connection indicates it is to be forwarded.
	 *
	 * See H323EndPoint documentation for more information.
	 */
	virtual BOOL OnConnectionForwarded (H323Connection &, const PString &forward_party, const H323SignalPDU &);

	/**
	 * @short Signal indicating that I am calling remote party
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual BOOL OnAlerting( H323Connection & connection, const H323SignalPDU & alertingPDU, const PString & user );

	/**
	 * @short Signal indicating that remote party wants to talk with me.
	 * 
	 * This is just a callback. See H323EndPoint documentation.\n 
	 * It gives an opportunity for a user to react on the call
	 * (unlike #OnIncomingCall). It displays AnswerCallWidget.
	 */
	virtual H323Connection::AnswerCallResponse OnAnswerCall(
			H323Connection &connection,
			const PString &caller,
			const H323SignalPDU & signalPDU,
			H323SignalPDU &);

	/**
	 * @short Signal indicating that remote party wants to talk with me.
	 * 
	 * This is just a callback. See H323EndPoint documentation.\n
	 * It just always returns true. It must be fast so there should not be
	 * any user reaction.
	 */
	virtual BOOL OnIncomingCall( H323Connection & connection, const H323SignalPDU & setupPDU, H323SignalPDU & alertingPDU );

	/**
	 * @short Signal indicating that a logical channel was opened
	 * 
	 * This is just a callback. See H323EndPoint documentation.\n
	 * It is called when video, audio or some other channel was opened.  In
	 * this implementation it does nothing. It just informs us about these
	 * events.
	 */
	virtual BOOL OnStartLogicalChannel(H323Connection & connection, H323Channel & channel); 

	/**
	 * @short Callback from the RTP session for statistics monitoring
	 * 
	 * This is just a callback. See H323EndPoint documentation.
	 */
	virtual void OnRTPStatistics( const H323Connection & connection, const RTP_Session & session ) const;

#ifndef NO_CALL_DIVERSIONS
	/**
	 * @name call_diversion_group
	 * Callbacks to hook call diversion.
	 */
	//@{
	virtual void OnOutgoingCallDiversionInProgress (H323Connection &, 
			H323Connection::CallDiversionReason diversionReason,
			const PString *nominatedParty,
			const PString *nominatedInfo,
			const PString *redirectingParty,
			const PString *redirectingInfo,
			const H4503_CDDivertingLegInformation1Arg &);
	virtual void OnOutgoingCallDiversionCompleted (H323Connection &, 
			bool presentationAllowed,
			const PString *redirectionParty,
			const PString *redirectionInfo,
			const H4503_CDDivertingLegInformation3Arg &);
	virtual void OnIncomingDivertedCall (H323Connection &, 
			int diversionCounter,
			H323Connection::CallDiversionReason diversionReason,
			const H323Connection::CallDiversionReason *originalDiversionReason,
			const PString *divertingParty,
			const PString *originalCalledParty,
			const PString *redirectingInfo,
			const PString *originalCalledInfo,
			const H4503_CDDivertingLegInformation2Arg &);
	virtual void OnActivateDiversionComplete (
			H323Connection &connection,
			bool succeeded,
			const H4503Handler::Error *error=0 );
	virtual void OnDeactivateDiversionComplete (
			H323Connection &connection,
			bool succeeded,
			const H4503Handler::Error *error=0 );
	virtual void OnInterrogateDiversionComplete (
			H323Connection &connection,
			bool succeeded,
			H4503Handler::InterrogateDiversionResultList *resultList,
			const H4503Handler::Error *error=0 );
	//@}
#endif

#ifndef NO_HTTP_SERVICE
	virtual void OnHTTPServiceControl( H323ServiceControlSession::ChangeType operation, unsigned sessionId, const PString & url );
#endif

	PConfigArgs& args;

#ifdef ENTERPRISE
	bool ldapQueried;
	PConfig ldapAttrs;
	PStringArray ldapAliases;
#endif

	/** @short Start ringing.  */
	void startRinging();
	/** @short Stop ringing.  */
	void stopRinging();

protected:
	H323ListenerTCP *listener;
	
	void setCurrentCallToken( const PString& token );
	void setTransferCallToken( const PString& token );
	void setHeldCallToken( const PString& token );
	PString& currentCallToken();
	PString& transferCallToken();
	PString& heldCallToken();

	virtual void handleRinging();

	bool gkForwardUnconditionaly;
	bool gkForwardNoResponse;
	bool gkForwardBusy;

private:
	EndPointPrivate *d;

};
extern EndPoint *endPoint;

#endif
