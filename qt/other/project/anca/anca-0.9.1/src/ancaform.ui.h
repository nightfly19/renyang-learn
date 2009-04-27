/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "anca.h"
#include "ancaconf.h"
#include "ancainfo.h"
#include "stdinfo.h"
#include "infomanager.h"
#include "configuredialog.h"
#include "confdefines.h"

#include "pluginfactory.h"

#include <qstatusbar.h>
#include <qprogressbar.h>

/**
 * @class AncaForm
 * @short The main window of the application.
 */
AncaForm *ancaForm = 0;

/**
 * @short Constructor of the AncaForm.
 */
void AncaForm::init() 
{
	ancaForm = this;
	
	calling = false;
	connect( callButton, SIGNAL( clicked() ), ConnectAction, SIGNAL( activated() ) );
	connect( callEdit, SIGNAL( returnPressed() ), ConnectAction, SIGNAL( activated() ) );
	
	connect( infoManager, SIGNAL(message( const QString& )), this, SLOT( message( const QString& )) );
	ancaInfo->connectNotify( ADDRESS_TO_CALL, this, SLOT( setCallAddress( const QVariant& )) );
	ancaInfo->connectNotify( ADDRESS_TO_CALL_NOW, this, SLOT( call( const QVariant& )) );
	
	pluginsMenu = new QPopupMenu(this, "pluginsMenu");
	mainButton->setPopup(pluginsMenu);
	mainButton->hide();
	
	buttonGroup->hide();
	
	ancaConf->installNotify( USE_BUTTON_LABELS, this, SLOT( useLabelsChanged() ) );
	ancaConf->installNotify( LABEL_POSITION_BELLOW_ICON, this, SLOT( labelPositionChanged() ) );
	
	ancaInfo->connectNotify( CALL_DURATION, this, SLOT(setDuration( const QVariant& )) );
	ancaInfo->connectNotify( PARTY_NAME, this, SLOT(setPartyName( const QVariant& )) );
	ancaInfo->connectNotify( PARTY_HOST, this, SLOT(setPartyHost( const QVariant& )) );
	ancaInfo->connectNotify( PARTY_PICTURE, this, SLOT(setPartyPicture( const QVariant& )) );
	ancaInfo->connectNotify( TIME, this, SLOT(setTime( const QVariant& )) );
	ancaInfo->connectNotify( DATE, this, SLOT(setDate( const QVariant& )) );
	ancaInfo->connectNotify( ADDRESS, this, SLOT(setAddress( const QVariant& )) );

	ancaConf->installNotify( DO_NOT_DISTURB, this, SLOT( dndChanged() ) );
	ancaConf->installNotify( ALWAYS_FORWARD, this, SLOT( forwardChanged() ) );
	
	callingProgressBar = new QProgressBar(5 /*total steps*/);
	callingProgressBar->setPercentageVisible(false);
	hideCallingProgress();
	
	callMenu = new QPopupMenu(this, "callMenu");
	ConnectAction->addTo( callMenu );
	HoldCallAction->addTo( callMenu );
	TransferAction->addTo( callMenu );
	callMenu->insertSeparator();
	ToggleDNDAction->addTo( callMenu );
	ForwardAction->addTo( callMenu );

	// set dnd and forward according to ancaConf
	dndChanged();
	forwardChanged();
	
	configDlg = 0;
}

void AncaForm::destroy()
{
	ancaForm = 0;
}

/**
 * @short Show message to the user.
 * 
 * Currently it displays the message \p msg in status bar for 10 minutes.
 */
void AncaForm::message( const QString & msg )
{
	statusBar()->message(msg,10000);
	PTRACE( 6, "Message: " << msg.latin1() );
}

/**
 * @short Inser button to the button frame.
 *
 * This function is called when inserting new widgets from plugins.
 * @param pixmap Picture of the button.
 * @param name name of the button (just for Qt purposses, can be NULL)
 * @param id number with which we want the button to be associated.
 * @param first insert the button as the first button or as the last?
 */
void AncaForm::insertButton( QPixmap pixmap, const char *name, int id, bool first )
{
	QToolButton *button = new QToolButton(buttonFrame, name);
	//insert it as the first plugin button
	if( first ) 
		buttonFrameLayout->insertWidget(0, button);
	//insert it as the last plugin button
	else 
		buttonFrameLayout->insertWidget(buttonGroup->count(), button);
	buttonGroup->insert( button, id );
	button->setTextLabel(name);
	button->setIconSet( QIconSet(pixmap ) );
	button->setToggleButton(true);
	button->show();
	button->setUsesTextLabel( ancaConf->readBoolEntry( USE_BUTTON_LABELS, USE_BUTTON_LABELS_DEFAULT ) );
#ifndef Q_WS_QWS
	button->setTextPosition( 
			ancaConf->readBoolEntry( LABEL_POSITION_BELLOW_ICON, LABEL_POSITION_BELLOW_ICON_DEFAULT ) ? QToolButton::BelowIcon : QToolButton::BesideIcon );
#else
        button->setTextPosition( 
                        ancaConf->readBoolEntry( LABEL_POSITION_BELLOW_ICON, LABEL_POSITION_BELLOW_ICON_DEFAULT ) ? QToolButton::Under : QToolButton::Right );
#endif 
	
	idList.append(id);
}

/**
 * @short Remove button from the button frame.
 *
 * @param id id of the button we want to remove. It was set with
 * insertButton().
 */
void AncaForm::removeButton( int id )
{
	delete buttonGroup->find(id);
	
	idList.remove(id);
}

/**
 * @short User changed configuration of how the button labels are shown.
 */
void AncaForm::useLabelsChanged()
{
	bool utl = ancaConf->readBoolEntry( USE_BUTTON_LABELS );
	for( QValueList<int>::iterator it = idList.begin(); it != idList.end(); ++it ) {
		QButton *b = buttonGroup->find(*it);
		if( b->isA( "QToolButton" ) ) {
			QToolButton *tb = (QToolButton*)b;
			tb->setUsesTextLabel( utl );
		}
	}
}

/**
 * @short User changed configuration of how the button labels are shown.
 */
void AncaForm::labelPositionChanged()
{
	QToolButton::TextPosition tp =
#ifndef Q_WS_QWS
			ancaConf->readBoolEntry( LABEL_POSITION_BELLOW_ICON ) ? QToolButton::BelowIcon : QToolButton::BesideIcon;
#else
                        ancaConf->readBoolEntry( LABEL_POSITION_BELLOW_ICON ) ? QToolButton::Under : QToolButton::Right;
#endif 
	for( QValueList<int>::iterator it = idList.begin(); it != idList.end(); ++it ) {
		QButton *b = buttonGroup->find(*it);
		if( b->isA( "QToolButton" ) ) {
			QToolButton *tb = (QToolButton*)b;
			tb->setTextPosition( tp );
		}
	}
}

/**
 * @short Change the call button according to the current calling state.
 * @param calling Are we calling?
 */
void AncaForm::setCallButton( bool calling )
{
	if( calling ) {
		callButton->setIconSet(QIconSet(QPixmap::fromMimeSource("connected.png")));
		ConnectAction->setText("Disconnect");
	}
	else {
		callButton->setIconSet(QIconSet(QPixmap::fromMimeSource("disconnected.png")));
		ConnectAction->setText("Connect");
	}
	this->calling = calling;
}

/**
 * @short Set the address we want to call.
 */
void AncaForm::setCallAddress( const QVariant &address )
{
	callEdit->setText(address.toString());
}

/**
 * @short Call the user.
 */
void AncaForm::call( const QVariant &address )
{
	anca->call( address.toString().latin1() );
}

/**
 * @short Configure button was clicked.
 * 
 * If the configure dialog does not exist, create it. Show it then.
 */
void AncaForm::configButton_clicked()
{
	if( !configDlg ) {
#ifndef Q_WS_QWS
		configDlg = new ConfigureDialog(this);
#else
		configDlg = new ConfigureDialog(this, "configureDialog", false, Qt::WStyle_Customize | Qt::WStyle_NoBorder);
#endif
	}

#ifndef Q_WS_QWS
	configDlg->show();
#else
	configDlg->showFullScreen();
#endif
}


/**
 * Current time has changed.
 * @param to current time.
 */
void AncaForm::setTime( const QVariant & to )
{
	timeLabel->setText(to.toString());
}


/**
 * Current date has changed.
 * @param to current date.
 */
void AncaForm::setDate( const QVariant & to )
{
	dateLabel->setText(to.toString());
}


/**
 * Local user address has changed.
 * @param to user address.
 */
void AncaForm::setAddress( const QVariant & to )
{
	addressLabel->setText(to.toString());
}


/**
 * Remote party name has changed.
 * @param to remote party name
 */
void AncaForm::setPartyName( const QVariant & to )
{
	callNameLabel->setText(to.toString());
}

/**
 * Remote party address (host part) has changed.
 * @param to remote party host
 */
void AncaForm::setPartyHost( const QVariant & to )
{
	callHostLabel->setText(to.toString());
}

/**
 * Remote party picture has changed.
 * @param to remote party picture
 */
void AncaForm::setPartyPicture( const QVariant & to )
{
	QPixmap pix;
	if( to.toString() == "default" )
		pix = QPixmap::fromMimeSource("defaultPicture.png");
	else
		pix = QPixmap(to.toString());
	callPictureLabel->setPixmap(pix);
}


/**
 * Duration of the call has changed.
 * @param to duration of the call
 */
void AncaForm::setDuration( const QVariant & to )
{
	callDurationLabel->setText(to.toString());
}


/**
 * Set new video widget.
 * @param w new video widget
 */
void AncaForm::setVideoWidget( QWidget *w )
{
	if( !w ) return;
	
	w->reparent( callVideoFrame, QPoint(0,0) );
	callVideoFrameLayout->add(w);
	callLogoLabel->hide();
}

/**
 * Show the progress bar in status bar when connecting to the remote
 * party.
 */
void AncaForm::showCallingProgress()
{
	statusBar()->addWidget(callingProgressBar, 0, true);
	callingProgressBar->show();
	
	startTimer(200); /* 0.2 second */
}

/**
 * Hide the calling progress bar shown be showCallingProgress().
 */
void AncaForm::hideCallingProgress()
{
	statusBar()->removeWidget(callingProgressBar);
	callingProgressBar->hide();
	
	killTimers();
}

void AncaForm::timerEvent( QTimerEvent * )
{
	if( callingProgressBar->progress() == 5 )
		callingProgressBar->reset();
	
	callingProgressBar->setProgress(callingProgressBar->progress()+1);
}

void AncaForm::mousePressEvent( QMouseEvent * e )
{
	ButtonState s = (ButtonState)ancaConf->readIntEntry(CALL_OPTIONS_BUTTON, (int)CALL_OPTIONS_BUTTON_DEFAULT);
	//cursor is inside displayStack and info or call page is displayed...
	if( 
		(displayStack->visibleWidget() == infoPage || displayStack->visibleWidget() == callPage) &&
		(e->button() == s) &&
		(displayStack->geometry().contains(e->pos()))
	){
		callMenu->popup(e->globalPos());
	}
	
	QWidget::mousePressEvent(e);
}

/**
 * Connect action was activated. Call the user specified in call edit.
 */
void AncaForm::ConnectAction_activated()
{
	if( !calling ) {
		anca->call(callEdit->text().latin1());
	}
	else {
		anca->hangUp();
	}
}

/**
 * Transfer action was activated. Transfer call to the user specified in call edit.
 */
void AncaForm::TransferAction_activated()
{
	if( !anca->transferCall( callEdit->text().latin1() ) )
		message( "Call could not be transferred.");
}

/**
 * Hold call action was activated. Hold/Retrieve current call.
 */
void AncaForm::HoldCallAction_activated()
{
	if( anca->holdRetrieveCall() )
		HoldCallAction->setText("Retrieve Call");
	else
		HoldCallAction->setText("Hold Call");
}


/**
 * Do not disturb action was toggled.
 * @param b Shoud the DND mode be set?
 */
void AncaForm::ToggleDNDAction_toggled( bool b )
{
	ancaConf->writeBoolEntry( DO_NOT_DISTURB, b );
}


/**
 * Forward action was toggled.
 * @param b Forward all incoming calls?
 */
void AncaForm::ForwardAction_toggled( bool b )
{
	ancaConf->writeBoolEntry( ALWAYS_FORWARD, b );
}

void AncaForm::forwardChanged()
{
	ForwardAction->setOn( ancaConf->readBoolEntry(ALWAYS_FORWARD) );
}

void AncaForm::dndChanged()
{
	ToggleDNDAction->setOn( ancaConf->readBoolEntry(DO_NOT_DISTURB) );
}
