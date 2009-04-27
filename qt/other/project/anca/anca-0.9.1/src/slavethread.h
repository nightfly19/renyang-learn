#ifndef SLAVETHREAD_H
#define SLAVETHREAD_H

#include <ptlib.h>
#include <qvaluelist.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/** 
 * @short Executes some operations in second (slave) thread.
 *
 * These operations are serialized, so one usage can be when you need to
 * serialize two asynchronous operations. Other usage is that some operations 
 * (like registration with gatekeeper) does not block the main (GUI) thread.
 */
class SlaveThread: public PThread {
	PCLASSINFO( SlaveThread, PThread )
public:
	SlaveThread( PConfigArgs& args );

	~SlaveThread();

	/** Enumerates instructions that the class is able to perform. */
	enum Instruction { 
		RegisterWithGk, 	/**< Register with gatekeeper. */
#ifndef NO_CALL_DIVERSIONS
		InterrogateDiversion,	/**< Interrogate server diversions. */
		SetDiversion,		/**< Set server diversions according to AncaConf. */
#endif
		MakeInitialCall,	/**< Make call at the very start of the application to the user that was specified on a command line. */
#ifdef ENTERPRISE
		FetchLDAP,		/**< Get information from LDAP server. This is available only in enterprise application. */
#endif
		InitEndPoint,		/**< Initialize EndPoint. */
		Terminate 		/**< Terminate SlaveThread. */
	};

	/** Queue of instructions. */
	QValueList<Instruction> iQueue;

	/** Execute instruction \p i */
	void doInstruction( Instruction i );

protected:
	virtual void Main();

	void registerWithGk();
#ifndef NO_CALL_DIVERSIONS
	void setDiversion();
	void interrogateDiversion();
#endif
	void makeInitialCall();
#ifdef ENTERPRISE
	void fetchLDAP();
#endif
	void initEndPoint();

	PConfigArgs& args;
};
extern SlaveThread *slaveThread;

#endif
