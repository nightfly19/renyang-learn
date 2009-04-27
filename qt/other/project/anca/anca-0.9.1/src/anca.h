#ifndef _ANCA_H
#define _ANCA_H

#include <ptlib.h>
#include <qobject.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * @brief Main class of PWlib library necessary to run OpenH323 stuff.
 *
 * Contains basic methods that manage calls and some other:
 * 	- initialize OpenH323 library (#init)
 * 	- make a call (#call)
 * 	- hang up current call (#hangUp)
 * Often these methods are just wrappers for EndPoint calls.
 */
class Anca: public QObject, public PProcess
{
	Q_OBJECT
        PCLASSINFO(Anca, PProcess)
public:
        Anca();
        ~Anca();

	/** 
	 * @short Main executive function called when the process is created
	 * 
	 * PWlib requires this function. It does nothing.
	 */
        virtual void Main() {};

	/** 
	 * @short Initialize openh323 library
	 * 
	 * Creates EndPoint, initializes it and calls #registerGk
	 *
	 * @returns true if initilization was successful.
	 */
	bool init( PConfigArgs& args );

	void shutdown();

public slots:
	/**
	 * @short Tries to register with gatekeeper.
	 *
	 * Registers with gatekeeper using SlaveThread.
	 */
	void registerGk();

	/**
	 * @short Sets server side call diversions if available.
	 *
	 * See EndPoint::setDiversion() for more details.
	 */
	void setDiversion();

	/**
	 * @short Try to transfer a call.
	 *
	 * If there is a held call, then transfer it to current,
	 * otherwise transfer current call to \p remoteParty.
	 * @returns true if call was transfered.
	 */
	bool transferCall( const PString& remoteParty );

	/**
	 * @short Makes a call to a remote party.
	 *
	 * It is a shortcut for EndPoint::makeCall().
	 *
	 * @param remoteParty the general form for this parameter is
	 * [alias@][transport$]host[:port] where the default alias is the same
	 * as the host, the default transport is "ip" and the default port is
	 * 1720
	 *
	 * @returns false if the call could not be established (wrong remoteParty etc.)
	 */
	bool call( const PString& remoteParty );

	/**
	 * @short Holds or retrieve call.
	 *
	 * It is a shortcut for EndPoint::holdRetrieveCall().
	 *
	 * @returns true if H323EndPoint::HoldCall was successful.
	 */
	bool holdRetrieveCall();
	
	/**
	 * @short Hangs up current call
	 */
	void hangUp();
};
extern Anca *anca;


#endif
