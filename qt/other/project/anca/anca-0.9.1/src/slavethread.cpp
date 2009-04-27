#include "slavethread.h"

#include "endpoint.h"
#include "anca.h"

SlaveThread *slaveThread = 0;

#define QUEUE_MAX_SIZE 1000				// = infinite
PSemaphore pSem( QUEUE_MAX_SIZE, QUEUE_MAX_SIZE );	//producent semaphore
PSemaphore cSem( 0, QUEUE_MAX_SIZE );			//consumer semaphore
PMutex qMutex;						//queue mutex

SlaveThread::SlaveThread( PConfigArgs& args )
: PThread(4096, PThread::NoAutoDeleteThread, PThread::NormalPriority, "slaveThread"), args(args)
{
	slaveThread = this;
}

SlaveThread::~SlaveThread()
{
	slaveThread = 0;
}

/**
 * This is normal producent-consumer synchonization problem...
 */
void SlaveThread::Main() 
{
	Instruction instr;

	while( true ) {
		cSem.Wait();
		// fetch instruction
		qMutex.Wait();
		instr = iQueue.front();
		iQueue.pop_front();
		qMutex.Signal();

		pSem.Signal();

//		printf("consuming instruction %d\n", instr);

		// do instruction
		switch( instr ) {
			case RegisterWithGk:
				registerWithGk();
				break;
#ifndef NO_CALL_DIVERSIONS
			case SetDiversion:
				setDiversion();
				break;
			case InterrogateDiversion:
				interrogateDiversion();
				break;
#endif
			case MakeInitialCall:
				makeInitialCall();
				break;
			case InitEndPoint:
				initEndPoint();
				break;
#ifdef ENTERPRISE
			case FetchLDAP:
				fetchLDAP();
				break;
#endif
			case Terminate:
				return;
			default:
				break;
		}
	}
}

void SlaveThread::registerWithGk() 
{
	endPoint->registerGk();
}

#ifndef NO_CALL_DIVERSIONS
void SlaveThread::setDiversion()
{
	endPoint->setDiversion();
}

void SlaveThread::interrogateDiversion()
{
	endPoint->interrogateDiversion();
}
#endif

void SlaveThread::makeInitialCall()
{
	endPoint->makeCall( args[0] );
}

#ifdef ENTERPRISE
void SlaveThread::fetchLDAP()
{
	endPoint->fetchLDAP();
}
#endif

void SlaveThread::initEndPoint()
{
	endPoint->init();
}

void SlaveThread::doInstruction( Instruction i ) 
{
//	printf("producing instruction %d\n", i );
	pSem.Wait();

	qMutex.Wait();
	iQueue.push_back(i);
	qMutex.Signal();

	cSem.Signal();
}
