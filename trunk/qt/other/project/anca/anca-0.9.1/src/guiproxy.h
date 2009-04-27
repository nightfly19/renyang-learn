#ifndef GUIPROXY_H
#define GUIPROXY_H

#include <qobject.h>
#include <qstring.h>
#include <qevent.h>
#include <qwaitcondition.h>

/**
 * @short Call GUI operations from threads different to the main one.
 */
class GUIProxy: public QObject {
	Q_OBJECT
public:
	GUIProxy();
	~GUIProxy();

	/**
	 * All possible types of answers on incoming call.
	 */
	enum AnswerType { 
		Accepted,	/**< Call was accepted by the local user. */
		Rejected,	/**< Call was rejected by the local user. */
		Timeout		/**< Call was rejected because of timeout. */
	};

	/** 
	 * @short Show dialog that will inform user about incoming call.
	 * @param caller Who is calling.
	 * @returns Answer of the local user.
	 */
	AnswerType answerCall( const QString& caller );

	/**
	 * @short Show critical message.
	 * @param caption Caption of the dialog.
	 * @param message Message to be shown.
	 */
	void messageCritical( const QString& caption, const QString& message );

	/**
	 * @short Destroy answer call dialog (if shown).
	 */
	void clearAnswerCall();

protected:
	void customEvent( QCustomEvent * );

	struct ProxyData {
		QWaitCondition cond;
		int returnValue;
		QString string1;
		QString string2;
	};

	virtual void handleAnswerCallEvent( ProxyData *data );
	virtual void handleMessageCriticalEvent( ProxyData *data );

	enum ProxyEventType {
		ANSWER_CALL_EVENT = 1234,
		MESSAGE_CRITICAL = 1235
	};
};
extern GUIProxy *guiProxy;

#endif
