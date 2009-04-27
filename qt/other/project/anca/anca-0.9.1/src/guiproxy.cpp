#include "guiproxy.h"

#include "ancaform.h"
#include "answercalldlg.h"
#include "ancaconf.h"
#include "confdefines.h"

#include <qapplication.h>
#include <qlabel.h>
#include <qmessagebox.h>

GUIProxy *guiProxy = 0;
AnswerCallDlg *answerCallDlg = 0;

GUIProxy::GUIProxy() {
	guiProxy = this;
}

GUIProxy::~GUIProxy() {
	guiProxy = 0;
}

GUIProxy::AnswerType GUIProxy::answerCall( const QString& caller ) 
{
	ProxyData data;
	data.string1 = caller;

	QApplication::postEvent( this, new QCustomEvent((QEvent::Type)ANSWER_CALL_EVENT, (void*)&data) );

	data.cond.wait();
	return (GUIProxy::AnswerType)data.returnValue;
}

void GUIProxy::messageCritical( const QString& caption, const QString& message )
{
	ProxyData data;
	data.string1 = caption;
	data.string2 = message;

	QApplication::postEvent( this, new QCustomEvent((QEvent::Type)MESSAGE_CRITICAL, (void*)&data) );

	data.cond.wait();
}
	
void GUIProxy::clearAnswerCall()
{
	if( answerCallDlg ) {
		qApp->lock();
		// this should call reject()
		answerCallDlg->close();
		qApp->unlock();
	}
}

void GUIProxy::handleAnswerCallEvent( ProxyData *data ) 
{
	AnswerCallDlg *dlg = answerCallDlg = new AnswerCallDlg(ancaForm, "answerCallDlg", true);

	dlg->nameLabel->setText( data->string1 );
	dlg->setTimeout(ancaConf->readIntEntry( ANSWER_TIMEOUT, ANSWER_TIMEOUT_DEFAULT ));
	int ret = dlg->exec();
	if( ret == QDialog::Accepted )
		data->returnValue = Accepted;
	else if( dlg->wasTimeout() )
		data->returnValue = Timeout;
	else
		data->returnValue = Rejected;
	data->cond.wakeAll();
	delete dlg;

	answerCallDlg = 0;
}

void GUIProxy::handleMessageCriticalEvent( ProxyData *data )
{
	QMessageBox::critical( ancaForm, data->string1, data->string2 );
	data->cond.wakeAll();
}

void GUIProxy::customEvent( QCustomEvent *event ) 
{
	ProxyData *data;
	switch( event->type() ) {
		case ANSWER_CALL_EVENT:
			data = (ProxyData*)event->data();
			handleAnswerCallEvent(data);
			break;
		case MESSAGE_CRITICAL:
			data = (ProxyData*)event->data();
			handleMessageCriticalEvent(data);
			break;
		default:
			QObject::customEvent(event);
	}
}
