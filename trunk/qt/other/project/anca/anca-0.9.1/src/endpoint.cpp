#include "endpoint.h"
#include "ancaconf.h"
#include "ancainfo.h"
#include "stdinfo.h"
#include "ancaform.h"
#include "answercalldlg.h"
#include "guiproxy.h"
#include "guimanager.h"
#include "plugin.h"
#include "pluginfactory.h"
#include "confdefines.h"
#include "slavethread.h"

#include <ptclib/pldap.h>

EndPoint *endPoint = 0;
PMutex endPointMutex;
PSemaphore interMutex(0,1); //first Wait() will lock()

class CallTimer: public PTimer
{
	H323Connection *con;
public:
	void OnTimeout()
	{
		PTimeInterval duration = PTime() - con->GetConnectionStartTime();
		int rest = duration.GetSeconds();
		int hour, min, sec;

		hour = rest / 3600; rest = rest % 3600;
		min = rest / 60; rest = rest % 60;
		sec = rest;

		char buf[9];
		if( hour )
			sprintf(buf, "%02d:%02d:%02d", hour, min, sec );
		else
			sprintf(buf, "%02d:%02d", min, sec );
		ancaInfo->set( CALL_DURATION, buf );
	}

	void start( H323Connection *con_ )
	{
		con = con_;
		RunContinuous(500);
	}

	void stop()
	{
		Stop();
	}
};

class RingThread : public PThread
{
	PCLASSINFO(RingThread, PThread);
	
public:
	RingThread( EndPoint & ep )
		: PThread(1000, NoAutoDeleteThread), endpoint(ep)
	{ Resume(); }
	
	void Main()
	{ endpoint.handleRinging(); }
	
protected:
	EndPoint & endpoint;
};

class EndPointPrivate
{
public:
	EndPointPrivate():
#ifdef ENTERPRISE
		ldap(0),
#endif
		gkForwardCurrent(0),
		m_diversionInterrogated(false),
		ringThread(0)
	{}

#ifdef ENTERPRISE
	PLDAPSession *ldap;
#endif

	bool *gkForwardCurrent;
	bool m_diversionInterrogated;

	PString m_currentCallToken;
	PString m_transferCallToken;
	PString m_heldCallToken;

	CallTimer *callTimer;

	PThread *ringThread;
	PSyncPoint ringFlag;
};

// ***********************************************************************

EndPoint::EndPoint( PConfigArgs& args )
	: args(args),
#ifdef ENTERPRISE
	ldapQueried(false),
#endif
	gkForwardUnconditionaly(false),
	gkForwardNoResponse(false),
	gkForwardBusy(false),
	d( new EndPointPrivate )
{
	PAssert( !endPoint, "EndPoint already created!" );

	endPoint = this;
	Plugin::endPoint = this;

	// install some statistics notification, see OnRTPStatistics()
	ancaInfo->install( JITTER_BUFFER_SIZE );
	ancaInfo->install( JITTER_AVERAGE_TIME );
	ancaInfo->install( JITTER_MAXIMUM_TIME );
	ancaInfo->install( TRANSMITTER_SESSION_INTERVAL );
	ancaInfo->install( RECEIVER_SESSION_INTERVAL );
	ancaInfo->install( PACKETS_SENT );
	ancaInfo->install( PACKETS_RECEIVED );
	ancaInfo->install( PACKETS_LOST );
	ancaInfo->install( PACKETS_TOO_LATE );

	d->callTimer = new CallTimer();
#ifdef ENTERPRISE
	d->ldap = new PLDAPSession( /*const PString& defaultBaseDN */ );
#endif

	// set up basic alias
	QStringList aliases = ancaConf->readListEntry( USER_ALIASES );
	if( aliases.isEmpty() ) {
		aliases.append( ( const char * ) PProcess::Current().GetUserName() );
		ancaConf->writeListEntry( USER_ALIASES, aliases );
	}
}


// ***********************************************************************

EndPoint::~EndPoint()
{
	if ( listener )
		RemoveListener( listener );

	delete d->callTimer;
#ifdef ENTERPRISE
	delete d->ldap;
#endif
	delete d;

	endPoint = 0;
}

// ***********************************************************************

bool EndPoint::init()
{
	PTRACE( 1, "Initializing endpoint" );

#ifdef ENTERPRISE
	if( !ldapQueried ) {
		PError << "LDAP was not queried successfuly." << endl;
		return false;
	}

	if( ldapAliases.IsEmpty() ) {
		PError << "Aliases were not fetched from LDAP." << endl;
		return false;
	}

	SetLocalUserName( ldapAliases[0] );
	for (int i = 1; i < ldapAliases.GetSize(); i++)
		AddAliasName( ldapAliases[i]);
	if( ldapAttrs.HasKey("h323IdentitydialedDigits") )
		AddAliasName( ldapAttrs.GetString("h323IdentitydialedDigits") );
#else

	if( args.HasOption('u')) {
		PStringArray aliases = args.GetOptionString('u').Lines();
		SetLocalUserName(aliases[0]);
		for (int i = 1; i < aliases.GetSize(); i++)
			AddAliasName(aliases[i]);
	}
	else {
		QStringList userAliases = ancaConf->readListEntry( USER_ALIASES );
		SetLocalUserName( userAliases[ 0 ].latin1() );
		for ( unsigned i = 1; i < userAliases.size(); i++ )
			AddAliasName( userAliases[ i ].latin1() );
	}
#endif

	bool disableFastStart = !ancaConf->readBoolEntry( FAST_START, FAST_START_DEFAULT );
	bool disableTunneling = !ancaConf->readBoolEntry( H245_TUNNELING, H245_TUNNELING_DEFAULT );
	bool disableH245InSetup = !ancaConf->readBoolEntry( H245_IN_SETUP, H245_IN_SETUP_DEFAULT );
	if( args.HasOption("fast-disable") )
		disableFastStart = true;
	if( args.HasOption("h245tunneldisable") )
		disableTunneling = true;
	if( args.HasOption("disable-h245-in-setup") )
		disableH245InSetup = true;

	DisableFastStart( disableFastStart );
	DisableH245Tunneling( disableTunneling );
	DisableH245inSetup( disableH245InSetup );


	// Codecs
	AudioPlugin *audioPlugin = (AudioPlugin*)pluginFactory->getPlugin(Plugin::Audio);
	if( audioPlugin ) {
		audioPlugin->addCapabilities();
	}

	VideoInPlugin *videoPlugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
	if( videoPlugin ) {
		videoPlugin->addCapabilities();
	}

	AddAllUserInputCapabilities( 0, 1 );

	PTRACE( 1, "Capabilities:\n" << setprecision( 2 ) << capabilities );

	ancaInfo->set( ADDRESS, ( const char * ) (GetLocalUserName() + "@" + PIPSocket::GetHostName()) );

	// Initialise the security info
	if( args.HasOption("password") ) {
		SetGatekeeperPassword( args.GetOptionString("password") );
		PTRACE( 1, "Enabling H.235 security access to gatekeeper." );
	}

	// Do not start H323 Listener
	if( args.HasOption("no-listenport") ) return true;

	// Start H323 Listener
	PIPSocket::Address addr = INADDR_ANY;
	WORD port = ancaConf->readIntEntry( PORT, PORT_DEFAULT );
	if (args.HasOption("listenport"))
		port = (WORD)args.GetOptionString("listenport").AsInteger();

	listener = new H323ListenerTCP( *this, addr, port );
	if ( listener == NULL || !StartListener( listener ) ) {
		PTRACE(1, "Unable to start H323 Listener at port " << port );
		if ( listener != NULL )
			delete listener;
		return false;
	}
	PTRACE( 1, "TCP listener started at port " << port );

	return true;
}

// ***********************************************************************

#ifdef ENTERPRISE
void EndPoint::fetchLDAP()
{
	PTRACE( 6, "Fetching information from LDAP" );

	ldapQueried = false;

	PLDAPSession *ldap = d->ldap;

	QString server = ancaConf->readEntry( ADMIN_GROUP, "LDAPServer", "localhost" );
	int port = ancaConf->readIntEntry( ADMIN_GROUP, "LDAPPort", 389 );
	QString base = ancaConf->readEntry( ADMIN_GROUP, "LDAPH323SearchBase", "" );

	if( server.isEmpty() ) return;

	PLDAPSession::SearchContext cnt;
	PStringList attrs;
	attrs.AppendString("commOwner");
	attrs.AppendString("h323IdentityGKDomain");
	attrs.AppendString("h323Identityh323-ID");
	attrs.AppendString("h323IdentitydialedDigits");
	attrs.AppendString("h323IdentitytransportID");
	QString filter;

	if( !ldap->IsOpen() && !ldap->Open( server.latin1(), port ) ) {
		PTRACE( 1, "Couldn't open connection to LDAP server: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection to LDAP server open.");

	filter = QString( "(&(objectClass=h323Identity)(commUniqueId=%1))" ).arg( (const char *)PIPSocket::GetHostName() );

	PTRACE( 3, "Search query: " << filter.latin1() );
	if( !ldap->Search( cnt, filter.latin1(), attrs, base.latin1()/*, PLDAPSession::ScopeSubTree*/ ) ) {
		PTRACE( 1, "Search query failed: " << ldap->GetErrorText() );
		goto out;
	}
	else
		PTRACE( 6, "Query successful.");

	for( int i=0; i < attrs.GetSize(); i++ ) {
		PString attr = attrs[i];
		if( attr == "h323Identityh323-ID" ) {
			if( ldap->GetSearchResult( cnt, attr, ldapAliases ) ) {
				PTRACE( 6, attr << ": " << ldapAliases );
			}
		}
		else {
			PString result;
			if( ldap->GetSearchResult( cnt, attr, result ) ) {
				ldapAttrs.SetString(attr, result);
				PTRACE( 6, attr << ": " << result );
			}
		}
	}

	ldapQueried = true;

out:
	if( !ldap->Close() ) {
		PTRACE( 1, "Couldn't close connection: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection closed");
}
#endif

// ***********************************************************************

bool EndPoint::registerGk()
{
#ifndef ENTERPRISE
	if( IsRegisteredWithGatekeeper() || args.HasOption("no-gatekeeper") )
#else
	if( IsRegisteredWithGatekeeper() )
#endif
		return false;
	

	bool gkResult = false;
#ifndef ENTERPRISE
	if( ancaConf->readBoolEntry( REGISTER_WITH_GK, REGISTER_WITH_GK_DEFAULT ) ||
			args.HasOption("gatekeeper") ||
			args.HasOption("gatekeeper-id") ) {

		RegistrationMethod m = (RegistrationMethod)ancaConf->readIntEntry( GK_REGISTRATION_METHOD, GK_REGISTRATION_METHOD_DEFAULT );
		PString gkAddr = ancaConf->readEntry( GK_ADDR ).latin1();
		PString gkId = ancaConf->readEntry( GK_ID ).latin1();

		bool useGk = false;
		if( args.HasOption("gatekeeper") ) {
			gkAddr = args.GetOptionString("gatekeeper");
			useGk = true;
		}
		if( args.HasOption("gatekeeper-id") ) {
			gkId = args.GetOptionString("gatekeeper-id");
			useGk = true;
		}

		//notify user that we are going to register with gatekeeper
		ancaInfo->setInt( GK_STATE, GK_REGISTERING );

		if( useGk )
			gkResult = UseGatekeeper( gkAddr, gkId );
		else
			switch( m ) {
				case GkAuto:
					gkResult = DiscoverGatekeeper();
					break;
				case GkAddr:
					gkResult = SetGatekeeper( gkAddr );
					break;
				case GkId:
					gkResult = LocateGatekeeper( gkId );
					break;
				case GkBoth:
					gkResult = SetGatekeeperZone( gkAddr, gkId );
				default:
					break;
			}
#else
		if( !ldapQueried || !ldapAttrs.HasKey( "h323IdentityGKDomain" ) ) {
			PError << "Gatekeeper domain was not fetched from LDAP." << endl;
			return false;
		}

		//notify user that we are going to register with gatekeeper
		ancaInfo->setInt( GK_STATE, GK_REGISTERING );

		gkResult = UseGatekeeper( ldapAttrs.GetString("h323IdentityGKDomain") );
#endif

		//notify user about the result
		if ( gkResult ) {
			PTRACE( 1, "Registred with gatekeeper: " << *gatekeeper );
			ancaInfo->setInt( GK_STATE, GK_REGISTERED );
			
#ifndef NO_CALL_DIVERSIONS
			// Activate server side diversions.
			// We don't need to call it by SlaveThread (as is written in documentation), 
			// because interrogation has not been done yet.. See setDiversion() code.
			if( ancaConf->readBoolEntry( USE_GK_DIVERSION, USE_GK_DIVERSION_DEFAULT ) )
				setDiversion();
#endif
		}
#ifdef ENTERPRISE
		else {
			PError << "Failed to register with gatekeeper." << endl;
			return false;
		}
#else
		else {
			PTRACE( 1, "Failed to register with gatekeeper");
			ancaInfo->setInt( GK_STATE, GK_NOT_REGISTERED );
		}
	}
	else {
		gkResult = RemoveGatekeeper();
		if ( !gkResult ) {
			PTRACE( 1, "Failed to remove gatekeeper" );
		} else {
			PTRACE( 1, "Gatekeeper removed" );
			ancaInfo->setInt( GK_STATE, GK_NOT_REGISTERED );
		}
	}
#endif

	return gkResult;
}

// ***********************************************************************

#ifndef NO_CALL_DIVERSIONS
void EndPoint::setDiversion()
{
	if( !IsRegisteredWithGatekeeper() ) return;

	if( !diversionInterrogated() ) {
		slaveThread->doInstruction( SlaveThread::InterrogateDiversion );
		slaveThread->doInstruction( SlaveThread::SetDiversion );
		return;
	}

	// Now we know, how is gatekeeper configured, so let's do just what we need.
	if( ancaConf->readBoolEntry( USE_GK_DIVERSION, USE_GK_DIVERSION_DEFAULT ) ) {
		if( ancaConf->readBoolEntry( ALWAYS_FORWARD, ALWAYS_FORWARD_DEFAULT ) != gkForwardUnconditionaly ) {
			d->gkForwardCurrent = &gkForwardUnconditionaly;
			if( gkForwardUnconditionaly ) {
				DeactivateDiversion( H323Connection::e_procedureCfu );
				interMutex.Wait();
			}
			else {
				ActivateDiversion( H323Connection::e_procedureCfu, ancaConf->readEntry( FORWARD_HOST ).latin1() );
				interMutex.Wait();
			}
		}
		if( ancaConf->readBoolEntry( NO_ANSWER_FORWARD, NO_ANSWER_FORWARD_DEFAULT ) != gkForwardNoResponse ) {
			d->gkForwardCurrent = &gkForwardNoResponse;
			if( gkForwardNoResponse ) {
				DeactivateDiversion( H323Connection::e_procedureCfnr );
				interMutex.Wait();
			}
			else {
				ActivateDiversion( H323Connection::e_procedureCfnr, ancaConf->readEntry( FORWARD_HOST ).latin1() );
				interMutex.Wait();
			}
		}
		if( ancaConf->readBoolEntry( BUSY_FORWARD, BUSY_FORWARD_DEFAULT ) != gkForwardBusy ) {
			d->gkForwardCurrent = &gkForwardBusy;
			if( gkForwardBusy ) {
				DeactivateDiversion( H323Connection::e_procedureCfb );
				interMutex.Wait();
			}
			else {
				ActivateDiversion( H323Connection::e_procedureCfb, ancaConf->readEntry( FORWARD_HOST ).latin1() );
				interMutex.Wait();
			}
		}
	}
	//Don't use any diversion
	else {
		if( gkForwardUnconditionaly ) {
			d->gkForwardCurrent = &gkForwardUnconditionaly;
			DeactivateDiversion( H323Connection::e_procedureCfu );
			interMutex.Wait();
		}
		if( gkForwardNoResponse ) {
			d->gkForwardCurrent = &gkForwardNoResponse;
			DeactivateDiversion( H323Connection::e_procedureCfnr );
			interMutex.Wait();
		}
		if( gkForwardBusy ) {
			d->gkForwardCurrent = &gkForwardBusy;
			DeactivateDiversion( H323Connection::e_procedureCfb );
			interMutex.Wait();
		}
	}
}

// ***********************************************************************

void EndPoint::interrogateDiversion()
{
	d->gkForwardCurrent = &gkForwardUnconditionaly;
	InterrogateDiversion( H323Connection::e_procedureCfu );
	interMutex.Wait();

	d->gkForwardCurrent = &gkForwardNoResponse;
	InterrogateDiversion( H323Connection::e_procedureCfnr );
	interMutex.Wait();

	d->gkForwardCurrent = &gkForwardBusy;
	InterrogateDiversion( H323Connection::e_procedureCfb );
	interMutex.Wait();

	setDiversionInterrogated(true);
}

// ***********************************************************************

void EndPoint::setDiversionInterrogated( bool set )
{
	PWaitAndSignal w(endPointMutex);

	d->m_diversionInterrogated = set;
}

// ***********************************************************************

bool EndPoint::diversionInterrogated()
{
	bool ret;

	endPointMutex.Wait();
	ret = d->m_diversionInterrogated;
	endPointMutex.Signal();

	return ret;
}
#endif //NO_CALL_DIVERSIONS

// ***********************************************************************

H323Connection * EndPoint::SetupTransfer(const PString & token, const PString &
		callIdentity, const PString & remoteParty, PString & newToken,
		void * /*userData*/) 
{
	H323Connection * conn = H323EndPoint::SetupTransfer(token, callIdentity, remoteParty, newToken);
	setTransferCallToken(newToken);
	return conn;
}

// ***********************************************************************

void EndPoint::OnConnectionEstablished( H323Connection & connection, const PString & token )
{
	PTRACE( 1, "Connection established, token is " << token );
	
	setCurrentCallToken(token);
	d->callTimer->start( &connection );

	PStringArray remote = connection.GetRemotePartyName().Tokenise("[]", false);
	ancaInfo->set( PARTY_NAME, (const char *)remote[0] );
	ancaInfo->set( PARTY_HOST, (const char *)remote[1] );

	ancaInfo->setInt( CALL_STATE, CALL_STARTED );
}


// ***********************************************************************

void EndPoint::OnConnectionCleared( H323Connection &connection, const PString &token )
{
	PAssertNULL( guiProxy );

	PTRACE( 1, "Connection cleared, token is " << token );
	
	// ignore connections that are not the current connection
	if( token != currentCallToken() ) return;

	// destroy answer call dialog (if shown)
	guiProxy->clearAnswerCall();

	// update values for current call token and call forward call token:
	if (!transferCallToken()) {
		// after clearing the first call during a call proceeding,
		// the call transfer call token becomes the new call token
		setCurrentCallToken( transferCallToken() );
		setTransferCallToken( PString() );
	}
	else
		setCurrentCallToken( PString() ); // indicate that our connection is now cleared

	d->callTimer->stop();
	
	ancaInfo->setInt( CALL_STATE, CALL_CLOSED );


	BOOL printDuration = TRUE;
	PString remoteName = "\"" + connection.GetRemotePartyName() + "\"";
	PString message;
	switch (connection.GetCallEndReason()) {
		case H323Connection::EndedByCallForwarded :
			printDuration = FALSE; // Don't print message here, was printed when forwarded
			break;
		case H323Connection::EndedByRemoteUser :
			message = remoteName + " has cleared the call";
			break;
		case H323Connection::EndedByCallerAbort :
			message = remoteName + " has stopped calling";
			break;
		case H323Connection::EndedByRefusal :
			message = remoteName + " did not accept your call";
			break;
		case H323Connection::EndedByRemoteBusy :
			message = remoteName + " was busy";
			break;
		case H323Connection::EndedByRemoteCongestion :
			message = "Congested link to " + remoteName;
			break;
		case H323Connection::EndedByNoAnswer :
			message = remoteName + " did not answer your call";
			break;
		case H323Connection::EndedByTransportFail :
			message = "Call with " + remoteName + " ended abnormally";
			break;
		case H323Connection::EndedByCapabilityExchange :
			message = "Could not find common codec with " + remoteName;
			break;
		case H323Connection::EndedByNoAccept :
			message = "Did not accept incoming call from " + remoteName;
			break;
		case H323Connection::EndedByAnswerDenied :
			message = "Refused incoming call from " + remoteName;
			break;
		case H323Connection::EndedByNoUser :
			message = "Gatekeeper could not find user " + remoteName;
			break;
		case H323Connection::EndedByNoBandwidth :
			message = "Call to " + remoteName + " aborted, insufficient bandwidth.";
			break;
		case H323Connection::EndedByUnreachable :
			message = remoteName + " could not be reached.";
			break;
		case H323Connection::EndedByHostOffline :
			message = remoteName + " is not online.";
			break;
		case H323Connection::EndedByNoEndPoint :
			message = "No phone running for " + remoteName;
			break;
		case H323Connection::EndedByConnectFail :
			message = "Transport error calling " + remoteName;
			break;
		default :
			message = "Call with " + remoteName + " completed";
	}
	ancaInfo->set( MESSAGE, (const char *)message );
}

// ***********************************************************************

BOOL EndPoint::OnConnectionForwarded (H323Connection &, const PString &forwardParty, const H323SignalPDU &)
{
	if (MakeCall (forwardParty, currentCallToken())) {
		PString message = "Call is being forwarded to \"" + forwardParty + "\"";
		ancaInfo->set( MESSAGE, (const char *)message );

		return TRUE;
	}
	
	PTRACE( 1, "Error forwarding call to \"" << forwardParty << "\"" );
	return FALSE;
}

// ***********************************************************************

BOOL EndPoint::OnAlerting( H323Connection &, const H323SignalPDU & /*alertingPDU*/, const PString & user )
{
	PTRACE( 1, "Alerting user " << user );

	return TRUE;
}

// ***********************************************************************

H323Connection::AnswerCallResponse EndPoint::OnAnswerCall( H323Connection & connection, 
		const PString & caller, const H323SignalPDU &, H323SignalPDU & ) 
{
	ancaInfo->set( CALLER_ADDRESS, (const char *)connection.GetRemotePartyAddress() );

	if( ancaConf->readBoolEntry( AUTO_ANSWER, AUTO_ANSWER_DEFAULT ) )
		return H323Connection::AnswerCallNow;

	startRinging();
	
	GUIProxy::AnswerType answer = guiProxy->answerCall( (const char *)caller );

	stopRinging();

	switch( answer ) {
		case GUIProxy::Accepted:
			PTRACE( 1, "Accepting connection from " << caller );
			return H323Connection::AnswerCallNow;

		case GUIProxy::Rejected:
			PTRACE( 1, "Connection from " << caller << " denied" );
			return H323Connection::AnswerCallDenied;

		case GUIProxy::Timeout:
			break;
	}

	// timeout happened

	QString forwardHost = ancaConf->readEntry( FORWARD_HOST );
	// forward
	if( args.HasOption("forward-no-answer") || 
			( !forwardHost.isEmpty() && ancaConf->readBoolEntry( NO_ANSWER_FORWARD, NO_ANSWER_FORWARD_DEFAULT ) ) ) {
		if( args.HasOption("forward-no-answer") )
			forwardHost = (const char *)args.GetOptionString("forward-no-answer");
		PTRACE( 1, "Forwarding call to " << forwardHost.latin1() << " (timeout)" );
		connection.ForwardCall (forwardHost.latin1());
		return H323Connection::AnswerCallPending;
	}
	// deny
	else {
		PTRACE( 1, "Connection from " << caller << " denied (timeout)" );
		return H323Connection::AnswerCallDenied;
	}
}

// ***********************************************************************

BOOL EndPoint::OnIncomingCall( H323Connection &connection, const H323SignalPDU &/*setupPDU*/, H323SignalPDU & /*alertingPDU*/ )
{
	//TODO messages

	QString forwardHost;
	QStringList forbiddenAliases = ancaConf->readListEntry(FORBIDDEN_ALIASES);
	bool reject = false;
	bool forward = false;

	if( ancaConf->readBoolEntry( DO_NOT_DISTURB, DO_NOT_DISTURB_DEFAULT ) ) {
		PTRACE(1, "Call rejected (DND)");
		reject = true;
	}
	//TODO forbiddenAliases should contain forbidden aliases
//	else if( forbiddenAliases.contains( (const char *)setupPDU.GetSourceAliases() ) ) {
//		reject = true;
//	}
	else if( args.HasOption("forward-always") || ancaConf->readBoolEntry( ALWAYS_FORWARD, ALWAYS_FORWARD_DEFAULT ) ) {
		PTRACE(1, "Call forwarded (always)");
		if( args.HasOption("forward-always") )
			forwardHost = (const char *)args.GetOptionString("forward-always");
		forward = true;
	}
	// incoming call is accepted if no call in progress
	else if( !currentCallToken().IsEmpty() ) {
		if( args.HasOption("forward-busy") || ancaConf->readBoolEntry( BUSY_FORWARD, BUSY_FORWARD_DEFAULT ) ) {
			if( args.HasOption("forward-busy") )
				forwardHost = (const char *)args.GetOptionString("forward-busy");
			PTRACE(1, "Call forwarded (busy)");
			forward = true;
		}
		else {
			PTRACE(1, "Call rejected (busy)");
			reject = true;
		}
	}

	if( forward ) {
		if( forwardHost.isNull() )
			forwardHost = ancaConf->readEntry( FORWARD_HOST );
		if( forwardHost.isEmpty() ) {
			PTRACE(1, "Call rejected (no forward host)");
			reject = true;
		}
		else
			return !connection.ForwardCall (forwardHost.latin1());
	}
	if( reject ) {
		connection.ClearCall (H323Connection::EndedByLocalBusy); 
		return FALSE;
	}

	// If we are here, the call doesn't need to be rejected or forwarded

	// get the current call token
	setCurrentCallToken( connection.GetCallToken() );
	return TRUE;
}

// ***********************************************************************

PString& EndPoint::transferCallToken() 
{
	PWaitAndSignal m(endPointMutex);

	return d->m_transferCallToken;
}

// ***********************************************************************

void EndPoint::setTransferCallToken( const PString& token ) 
{
	PWaitAndSignal m(endPointMutex);

	d->m_transferCallToken = token; 
}

// ***********************************************************************

PString& EndPoint::currentCallToken() 
{
	PWaitAndSignal m(endPointMutex);

	return d->m_currentCallToken;
}

// ***********************************************************************

void EndPoint::setCurrentCallToken( const PString& token ) 
{
	PWaitAndSignal m(endPointMutex);

	d->m_currentCallToken = token; 

	//inform plugins about the call
	Plugin::currentCallToken = (const char *)token;
}

// ***********************************************************************

PString& EndPoint::heldCallToken() 
{
	PWaitAndSignal m(endPointMutex);

	return d->m_heldCallToken;
}

// ***********************************************************************

void EndPoint::setHeldCallToken( const PString& token ) 
{
	PWaitAndSignal m(endPointMutex);

	d->m_heldCallToken = token; 
}

// ***********************************************************************

BOOL EndPoint::OnStartLogicalChannel( H323Connection &,
                                      H323Channel & channel )
{
	PString dir;
	switch ( channel.GetDirection() ) {
	case H323Channel::IsTransmitter :
		dir = "sending";
		break;
	case H323Channel::IsReceiver :
		dir = "receiving";
		break;
	default :
		break;
	}

	PTRACE( 1, "Started logical channel " << dir << " " << channel.GetCapability() );

	return true;
}


// ***********************************************************************

BOOL EndPoint::OpenAudioChannel(H323Connection & connection, BOOL isEncoding, unsigned bufferSize, H323AudioCodec & codec)
{
	codec.SetSilenceDetectionMode(GetSilenceDetectionMode());

	if ( isEncoding ) {
		if( !ancaConf->readBoolEntry( SEND_AUDIO, SEND_AUDIO_DEFAULT ) ) {
			PTRACE(1, "Sending audio is forbidden in configuration");
			return false;
		}
	}
	else {
		if( !ancaConf->readBoolEntry( RECEIVE_AUDIO, RECEIVE_AUDIO_DEFAULT ) ) {
			PTRACE(1, "Receiving audio is forbidden in configuration");
			return false;
		}
	}

	AudioPlugin *plugin = (AudioPlugin*)pluginFactory->getPlugin(Plugin::Audio);
	if( !plugin ) return false;
	
	plugin->prepareForCall();
	plugin->setBufferSize(bufferSize);
	
	PSoundChannel *channel = (PSoundChannel*)plugin->getChannel(isEncoding);
	if( !channel ) return false;
	
	PTRACE( 1, "Audio channel opened for " << (isEncoding ? "encoding" : "decoding") );

	return codec.AttachChannel(channel);
}

// ***********************************************************************

BOOL EndPoint::OpenVideoChannel( H323Connection &, BOOL isEncoding, H323VideoCodec & codec )
{
	if ( isEncoding ) {
		if( args.HasOption("no-videotransmit") ) return false;
		if( !args.HasOption("videotransmit") && !ancaConf->readBoolEntry( SEND_VIDEO, SEND_VIDEO_DEFAULT ) ) return false;

		VideoInPlugin *plugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
		if( !plugin ) return false;
		
		plugin->prepareForCall();
		
		PVideoChannel *channel = (PVideoChannel*)plugin->getChannel(true);
		if( !channel ) return false;
		
		PTRACE( 1, "Video channel opened for encoding" );

		/* false = do not delete channel on exit */
		return codec.AttachChannel( channel, false );
	}
	else {
		if( args.HasOption("no-videoreceive") ) return false;
		if( !args.HasOption("videoreceive") && !ancaConf->readBoolEntry( RECEIVE_VIDEO, RECEIVE_VIDEO_DEFAULT ) ) return false;

		VideoOutPlugin *pluginOut = (VideoOutPlugin*)pluginFactory->getPlugin(Plugin::VideoOut);
		if( !pluginOut ) return false;
		VideoInPlugin *pluginIn = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
		if( !pluginIn ) return false;
		
		PVideoChannel *channel = (PVideoChannel*)pluginIn->getChannel(false);
		if( !channel ) return false;
		
		QWidget *videoWidget = guiManager->getVideoWidget();
		if( !videoWidget ) return false;
		
		PVideoOutputDevice *device = pluginOut->createVideoOutputDevice(videoWidget);
		if( !device ) return false;

		PTRACE( 1, "Video channel opened for decoding" );

		channel->AttachVideoPlayer ( device );

		if ( channel )
			return codec.AttachChannel ( channel );
	}

	return false;
}

// ***********************************************************************

bool EndPoint::makeCall( const PString& remoteParty )
{
	H323Connection *con = MakeCall( remoteParty, currentCallToken() );

	// no error occured
	if( con && con->GetCallEndReason() == H323Connection::NumCallEndReasons ) {
		//set ADDRESS_CALLING and CALL_STATE standard info
		ancaInfo->set( ADDRESS_CALLING, (const char *)remoteParty );
		ancaInfo->setInt( CALL_STATE, CALL_CALLING );
		PTRACE( 1, "Calling to " << remoteParty << ", token is " << currentCallToken() );
		return true;
	}

	return false;
}

// ***********************************************************************

void EndPoint::hangUpCurrent()
{
	if( currentCallToken().IsEmpty() ) return;
	
	ClearCall( currentCallToken() );
	
	PTRACE( 1, "Hanging up current call, token is " << currentCallToken() );
}

// ***********************************************************************

void EndPoint::OnRTPStatistics( const H323Connection &, const RTP_Session & session ) const
{
	// jitter
	ancaInfo->setInt( JITTER_BUFFER_SIZE, session.GetJitterBufferSize() );
	ancaInfo->setInt( JITTER_AVERAGE_TIME, session.GetAvgJitterTime() );
	ancaInfo->setInt( JITTER_MAXIMUM_TIME, session.GetMaxJitterTime() );

	// number of packets sent and received in session
	ancaInfo->setInt( PACKETS_SENT, session.GetPacketsSent() );
	ancaInfo->setInt( PACKETS_RECEIVED, session.GetPacketsReceived() );

	// number of received packets lost in session
	ancaInfo->setInt( PACKETS_LOST, session.GetPacketsLost() );

	// number of received packets that were too late
	ancaInfo->setInt( PACKETS_TOO_LATE, session.GetPacketsTooLate() );

	// there could be many another statistics
}

// ***********************************************************************

bool EndPoint::holdRetrieveCall()
{
	// retrieve held call -> token stored in heldCallToken
	if( !heldCallToken() ) {
		// find appropriate connection
		H323Connection *con = FindConnectionWithLock( heldCallToken() );
		if( con ) {
			setCurrentCallToken( heldCallToken() );
			con->RetrieveCall();
			setHeldCallToken( PString() );
			con->Unlock();

			ancaInfo->setInt( CALL_STATE, CALL_STARTED );

			ancaInfo->set( MESSAGE, "Call was retrieved." );
			return false;
		}
	}
	// hold current call -> token stored in currentCallToken
	else if( !currentCallToken() ) {
		// find appropriate connection
		H323Connection *con = FindConnectionWithLock( currentCallToken() );
		if( con ) {
			setHeldCallToken(currentCallToken());
			con->HoldCall(true);
			setCurrentCallToken( PString() );
			con->Unlock();

			ancaInfo->setInt( CALL_STATE, CALL_HELD );

			ancaInfo->set( MESSAGE, "Call was held.");
			return true;
		}
	}

	PTRACE( 1, "No active nor held call, EndPoint::holdRetrieveCall could not be processed.");
	return false;
}

// ***********************************************************************

bool EndPoint::transferHeldCall()
{
	if (!currentCallToken() && !heldCallToken() ) {
		ancaInfo->set( MESSAGE, "Invoking consultation transfer." );
		ConsultationTransfer(heldCallToken(),currentCallToken());
		setHeldCallToken(PString());
		return true;
	}

	PTRACE( 1, "Consultation transfer could not be processed." );
	return false;
}

// ***********************************************************************

bool EndPoint::transferCall( const PString& remoteParty )
{
	if( !heldCallToken() ) {
		PTRACE( 1, "Call could not be transferred to " << remoteParty << 
				", because of the held call. Call EndPoint::transferHeldCall()");
		return false;
	}

	if( !currentCallToken() ) {
		if( remoteParty.IsEmpty() ) {
			PTRACE( 1, "Call could not be transferred, because remote party was not specified.");
			return false;
		}

		ancaInfo->set( MESSAGE, QString("Transferring call to ") + (const char *)remoteParty );
		TransferCall(currentCallToken(), remoteParty);
		return true;
	}

	PTRACE( 1, "No active call, EndPoint::transferCall could not be processed.");
	return false;
}

// ************ DIVERSIONS ************************************************

#ifndef NO_CALL_DIVERSIONS
void EndPoint::OnOutgoingCallDiversionInProgress (H323Connection &, 
		H323Connection::CallDiversionReason /*diversionReason*/,
		const PString * /*nominatedParty*/,
		const PString * /*nominatedInfo*/,
		const PString *redirectingParty,
		const PString * /*redirectingInfo*/,
		const H4503_CDDivertingLegInformation1Arg & /*Info*/)
{
	QString message = QString("Call is diverting");
	if (redirectingParty)
		message += " to: " + *redirectingParty;
	ancaInfo->set( MESSAGE, message );
}

void EndPoint::OnOutgoingCallDiversionCompleted (H323Connection &,
		bool presentationAllowed,
		const PString *redirectionParty,
		const PString * /*redirectionInfo*/,
		const H4503_CDDivertingLegInformation3Arg & /*Info*/)
{
	if (presentationAllowed) {
		QString message = "Call diverted";
		if (redirectionParty)
			message += " to: " + *redirectionParty;
		ancaInfo->set( MESSAGE, message );
	}
}

void EndPoint::OnIncomingDivertedCall (H323Connection &,
		int /*diversionCounter*/,
		H323Connection::CallDiversionReason /*diversionReason*/,
		const H323Connection::CallDiversionReason * /*originalDiversionReason*/,
		const PString *divertingParty,
		const PString *originalCalledParty,
		const PString * /*redirectingInfo*/,
		const PString * /*originalCalledInfo*/,
		const H4503_CDDivertingLegInformation2Arg & /*Info*/)
{
	QString message = QString("Diverted call");
	if (divertingParty)
		message += " by " + *divertingParty;
	if (originalCalledParty)
		message += " originally to: " + *originalCalledParty;
	ancaInfo->set( MESSAGE, message );
}

void EndPoint::OnActivateDiversionComplete (
		H323Connection &connection,
		bool succeeded,
		const H4503Handler::Error *error ) 
{
	H323EndPoint::OnActivateDiversionComplete (connection, succeeded, error);
	const char *errorStr = "(unknown error)"; if (error!=0) errorStr = *error; 
	if (succeeded) {
		QString message = "Call diversion activated";
		PTRACE( 3, message.latin1() );
		ancaInfo->set( MESSAGE, message );

		*d->gkForwardCurrent = true;
	}
	else {
		PTRACE( 3, "Call diversion not activated: "<< errorStr);
		*d->gkForwardCurrent = false;
	}

	interMutex.Signal();
}

void EndPoint::OnDeactivateDiversionComplete (
		H323Connection &connection,
		bool succeeded,
		const H4503Handler::Error *error ) 
{
	H323EndPoint::OnDeactivateDiversionComplete (connection, succeeded, error);
	const char *errorStr = "(unknown error)"; if (error!=0) errorStr = *error; 
	if (succeeded) {
		QString message = "Call diversion deactivated";
		PTRACE( 3, message.latin1() );
		ancaInfo->set( MESSAGE, message );

		*d->gkForwardCurrent = false;
	}
	else {
		PTRACE(3, "Call diversion not deactivated: "<< errorStr);
		*d->gkForwardCurrent = true;
	}

	interMutex.Signal();
}

void EndPoint::OnInterrogateDiversionComplete (
		H323Connection &connection,
		bool succeeded,
		H4503Handler::InterrogateDiversionResultList *resultList,
		const H4503Handler::Error *error ) 
{
	H323EndPoint::OnInterrogateDiversionComplete (connection, succeeded, resultList, error);
	const char *errorStr = "(unknown error)"; if (error!=0) errorStr = *error; 
	if (succeeded) {
		PINDEX len = resultList->GetSize();
		if (len) {
			PTRACE( 3, "Call diversion:");
			for (PINDEX i=0; i<len; i++)
			{
				//@@FIXME@@ vypisat v zrozumitelnejsom formate
				PTRACE( 3, resultList[i] );
			}
		} else
			PTRACE(3, "No active diversion.");

		PAssertNULL(d->gkForwardCurrent);
		*d->gkForwardCurrent = len;
	} 
	else
		PTRACE(1, "Interrogate call diversion failed: "<< errorStr);

	interMutex.Signal();
}
#endif //NO_CALL_DIVERSIONS

#ifndef NO_HTTP_SERVICE
void EndPoint::OnHTTPServiceControl( H323ServiceControlSession::ChangeType operation, unsigned /*sessionId*/, const PString & url )
{
	if( operation == H323ServiceControlSession::OpenSession || operation == H323ServiceControlSession::RefreshSession )
		ancaInfo->set( HTTP_PUSH, (const char *)url );
	else
		ancaInfo->set( HTTP_CLOSE, (const char *)url );
}
#endif

///////////////////////////// Ringing ///////////////////////////////

void EndPoint::handleRinging()
{
	PSoundChannel dev(GetSoundChannelPlayDevice(), PSoundChannel::Player);
	if (!dev.IsOpen()) {
		PTRACE(2, "Cannot open sound device for ring");
		return;
	}
	dev.SetFormat( 2, 44100, 16 );
	
	int ringDelay = ancaConf->readIntEntry( RING_DELAY, RING_DELAY_DEFAULT );
	PFilePath ringFile( ancaConf->readEntry( RING_FILE, RING_FILE_DEFAULT ).latin1() );

	if (ringDelay < 0) {
		PTRACE(2, "Playing " << ringFile);
		dev.PlayFile(ringFile, TRUE);
	} else {
		PTimeInterval delay(0, ringDelay);
		PTRACE(2, "Playing " << ringFile << " with repeat of " << delay << " s");
		do {
			dev.PlayFile(ringFile, TRUE);
		} while (!d->ringFlag.Wait(delay));
	}
}


void EndPoint::startRinging()
{
	PAssert(d->ringThread == NULL, "Ringing thread already present");
	
	QString defaultRingFile;
	if ( !ancaConf->readEntry( RING_FILE, RING_FILE_DEFAULT ).isEmpty() )
		d->ringThread = new RingThread(*this);
}


void EndPoint::stopRinging()
{
	if (d->ringThread == NULL)
		return;
	
	d->ringFlag.Signal();
	d->ringThread->WaitForTermination();
	delete d->ringThread;
	d->ringThread = NULL;
}

