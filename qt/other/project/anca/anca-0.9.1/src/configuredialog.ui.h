/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "pluginfactory.h"
#include "endpoint.h"
#include "ancaconf.h"
#include "confdefines.h"
#include "stdconf.h"
#include "configuredialog.ui.private.h"
#include "personaldialog.h"

#include <qptrlist.h>
#include <qlistbox.h>
#include <qinputdialog.h>

#include <ptclib/pldap.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/**
 * @class ConfigureDialog
 * @short The main configuration dialog of the application.
 *
 * There are six tabs (five in the enterprise application):
 * - Personal - contains all personal information that are published in LDAP directory
 * - General - some basic configuration on how the application shout behave
 * - Codecs - Audio & Video transmission configuration
 * - Interface - interface configuration
 * - Gatekeeper (not in enterprise) - gatekeeper configuration
 * - Plugins - plugins configuration
 * 
 * All changes are collected and applied after applyChanges(). Notification about
 * the change is done by configChanged().
 */

/**
 * Constructor of ConfigureDialog.
 */
void ConfigureDialog::init()
{
	d = new ConfigureDialogPrivate();

	d->personalChanged = false;
	d->disableChanges = false;

	//create audioGroupBoxLayout the same way as videoGroupBoxLayout is automaticaly
	audioGroupBox->setColumnLayout(0, Qt::Vertical );
	audioGroupBox->layout()->setSpacing( 6 );
	audioGroupBox->layout()->setMargin( 11 );
	audioGroupBoxLayout = new QVBoxLayout( /*audioGroupBox, 0, 6*/ audioGroupBox->layout() );
	audioGroupBoxLayout->setAlignment( Qt::AlignTop );

	insertAudioConfig();
	insertVideoConfig();

	setupValues();

	ancaConf->installNotify( LIBRARY_PATHS, this, SLOT( displayPlugins() ) );

#ifdef ENTERPRISE
	gkGroupBoxLayout->remove( useDiversionCheckBox );
	useDiversionCheckBox->reparent( forwardBox, 0, QPoint(0,0) );
	forwardBoxLayout->addWidget( useDiversionCheckBox );
	useDiversionCheckBox->show();

	configTabWidget->removePage(TabPage_5); //remove gatekeeper tab
#else
	configTabWidget->removePage(TabPage); //remove personal information tab (proprietary solution for now)
#endif

#ifdef NO_CALL_DIVERSIONS
	useDiversionCheckBox->hide();
#endif
}

/**
 * Set all values according to AncaConf.
 */
void ConfigureDialog::setupValues()
{
	d->disableChanges = true;

#ifdef ENTERPRISE //this is proprietary solution for now
	// personal
	updatePersonalTab();
#endif

	// general
	forwardHostEdit->setText( ancaConf->readEntry( FORWARD_HOST ) );
	busyCheckBox->setChecked( ancaConf->readBoolEntry( BUSY_FORWARD, BUSY_FORWARD_DEFAULT ) );
	noAnswerCheckBox->setChecked( ancaConf->readBoolEntry( NO_ANSWER_FORWARD, NO_ANSWER_FORWARD_DEFAULT ) );
	alwaysCheckBox->setChecked( ancaConf->readBoolEntry( ALWAYS_FORWARD, ALWAYS_FORWARD_DEFAULT ) );
	dndCheckBox->setChecked( ancaConf->readBoolEntry( DO_NOT_DISTURB, DO_NOT_DISTURB_DEFAULT ) );
	autoAnswerCheckBox->setChecked( ancaConf->readBoolEntry( AUTO_ANSWER, AUTO_ANSWER_DEFAULT ) );

	int timeout = ancaConf->readIntEntry( ANSWER_TIMEOUT, ANSWER_TIMEOUT_DEFAULT );
	if( endPoint->args.HasOption("answer-timeout") )
		timeout = endPoint->args.GetOptionString("answer-timeout").AsInteger();
	answerSpinBox->setValue( timeout );

	sendVideoCheckBox->setChecked( ancaConf->readBoolEntry( SEND_VIDEO, SEND_VIDEO_DEFAULT ) );
	receiveVideoCheckBox->setChecked( ancaConf->readBoolEntry( RECEIVE_VIDEO, RECEIVE_VIDEO_DEFAULT ) );

	// interface
	pluginButtonsSpinBox->setValue( ancaConf->readIntEntry( MAX_DIRECTLY_VISIBLE_PLUGINS, MAX_DIRECTLY_VISIBLE_PLUGINS_DEFAULT ) );
	Qt::ButtonState s = (Qt::ButtonState)ancaConf->readIntEntry( CALL_OPTIONS_BUTTON, CALL_OPTIONS_BUTTON_DEFAULT );
	switch( s ) {
		case Qt::LeftButton: optionsButtonTypeComboBox->setCurrentItem( 0 ); break;
		case Qt::MidButton: optionsButtonTypeComboBox->setCurrentItem( 1 ); break;
		case Qt::RightButton: optionsButtonTypeComboBox->setCurrentItem( 2 ); break;
		default: break;
	}
	labelPositionCheckBox->setChecked( ancaConf->readBoolEntry( USE_BUTTON_LABELS, USE_BUTTON_LABELS_DEFAULT ) );
	labelPositionComboBox->setCurrentItem( ancaConf->readBoolEntry( LABEL_POSITION_BELLOW_ICON, LABEL_POSITION_BELLOW_ICON_DEFAULT ) ? 0 : 1 );

	// gatekeeper
	useGkCheckBox->setChecked( ancaConf->readBoolEntry( REGISTER_WITH_GK, REGISTER_WITH_GK_DEFAULT ) );
	gkMethodComboBox->setCurrentItem( ancaConf->readIntEntry( GK_REGISTRATION_METHOD, GK_REGISTRATION_METHOD_DEFAULT ) );
	gkAddressEdit->setText( ancaConf->readEntry( GK_ADDR ) );
	gkIdEdit->setText( ancaConf->readEntry( GK_ID ) );
	useDiversionCheckBox->setChecked( ancaConf->readBoolEntry( USE_GK_DIVERSION, USE_GK_DIVERSION_DEFAULT ) );

	// plugins
	displayPlugins();

	applyButton->setEnabled(false);
	d->disableChanges = false;
}

/**
 * @short Apply changes that were done to configuration since last applyChanges().
 *
 * This is done by iterating list of ConfigureAction classes and calling
 * ConfigureAction::apply() function on each. Signal onApplyChanges is
 * emitted after it is done.
 */
void ConfigureDialog::applyChanges()
{
	for( ConfigureAction *a = d->configList.first(); a; a = d->configList.next() )
		a->apply();
	d->configList.clear();
	d->configKeyMap.clear();

	if( d->personalChanged ) {
		setPersonalTab();
	}

	applyButton->setEnabled(false);

	emit onApplyChanges();
}

/**
 * Enable apply button.
 */
void ConfigureDialog::setChanged()
{
	applyButton->setEnabled(true);
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 */
void ConfigureDialog::configChanged( const QString& key, const QString& value )
{
	configChanged(key,value,false);
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 * @param notifySameMore if true all listeners connected to this config item
 * will be informed about the change even when the new value is the same as
 * the old one.
 */
void ConfigureDialog::configChanged( const QString& key, const QString& value, bool notifySameMore )
{
	//printf("configChanged => key: %s, value: %s, notifySameMore: %d\n", key.latin1(), value.latin1(), notifySameMore );
	if( d->disableChanges ) return;

	ConfigureAction *action = 0;

	if( d->configKeyMap.contains(key) )
		action = d->configKeyMap[key];
	//create new ConfigureAction
	else {
		action = new ConfigureAction(key);
		d->configKeyMap[key] = action;
		d->configList.append(action);
	}

	action->set(value, notifySameMore);

	setChanged();
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 */
void ConfigureDialog::configChanged( const QString & key, bool value )
{
	configChanged(key,value,false);
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 * @param notifySameMore if true all listeners connected to this config item
 * will be informed about the change even when the new value is the same as
 * the old one.
 */
void ConfigureDialog::configChanged( const QString & key, bool value, bool notifySameMore )
{
	configChanged( key, QString::number(value), notifySameMore );
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 */
void ConfigureDialog::configChanged( const QString & key, int value )
{
	configChanged(key,value,false);
}

/**
 * Configuration of the config item was changed.
 * @param key key of the AncaConf of the config item that was changed
 * @param value to what value the config item was changed
 * @param notifySameMore if true all listeners connected to this config item
 * will be informed about the change even when the new value is the same as
 * the old one.
 */
void ConfigureDialog::configChanged( const QString & key, int value, bool notifySameMore )
{
	configChanged( key, QString::number(value), notifySameMore );
}

/**
 * @short Cancel button was clicked.
 *
 * Clear the ConfigureAction list and reject the dialog.
 * Signal onRejectChanges is emitted.
 */
void ConfigureDialog::cancelButton_clicked()
{
	d->configList.clear();
	d->configKeyMap.clear();

//	setupValues();

	reject();

	emit onRejectChanges();
}


/**
 * OK button was clicked. Apply changes and accept dialog.
 */
void ConfigureDialog::okButton_clicked()
{
	applyChanges();

	accept();
}

/* ------------------ plugins section -------------------- */
void ConfigureDialog::pluginsView_selectionChanged( QListViewItem *i )
{
	if( !i ) {
		pluginDescFrame->setEnabled(false);
	}
	else {
		pluginDescFrame->setEnabled(true);
		PluginsViewItem *item = (PluginsViewItem*)i;
		QString note;
		if( !item->canUnload() ) note = "\n\nNOTE: If you uncheck this plugin, it will not be loaded next time the applicating is started.";
		inputDescLabel->setText(item->description() + note);
		configurePluginButton->setEnabled( item->loaded() && item->hasConfig() );
	}
}

void ConfigureDialog::displayPlugins()
{
	pluginsView->clear();
	
	PluginInfoList& plugins = pluginFactory->rescan();
	for( PluginInfoList::iterator it = plugins.begin(); it != plugins.end(); ++it ) {
		new PluginsViewItem( this, pluginsView, *it );
	}
}

void ConfigureDialog::configurePluginButton_clicked()
{
	QListViewItem *i = pluginsView->selectedItem();
	if( !i ) return;
	
	PluginsViewItem *item = (PluginsViewItem*)i;
	QDialog *dlg = item->info()->get()->createConfig(this);
	if( !dlg ) return;
	
	dlg->show();
}


/* ---------------- interface section ------------------------ */
void ConfigureDialog::pluginButtonsSpinBox_valueChanged( int value )
{
	configChanged( MAX_DIRECTLY_VISIBLE_PLUGINS, value );
}

void ConfigureDialog::labelPositionCheckBox_toggled( bool on )
{
	configChanged( USE_BUTTON_LABELS, on );
}

void ConfigureDialog::labelPositionComboBox_activated( int index )
{
	configChanged( LABEL_POSITION_BELLOW_ICON, index == 0 );
}

void ConfigureDialog::optionsButtonTypeComboBox_activated( int i )
{
	int buttonType;
	
	switch( i ) {
		case 0: buttonType = Qt::LeftButton; break;
		case 1: buttonType = Qt::MidButton; break;
		case 2: buttonType = Qt::RightButton; break;
	}
	configChanged( CALL_OPTIONS_BUTTON, buttonType );
}

/* ---------------- gatekeeper section ----------------------- */

void ConfigureDialog::useGkCheckBox_toggled( bool b )
{
	configChanged( REGISTER_WITH_GK, b );
}

void ConfigureDialog::gkMethodComboBox_activated( int i )
{
	configChanged( GK_REGISTRATION_METHOD, i );
	configChanged( REGISTER_WITH_GK, useGkCheckBox->isChecked(), true );
}

void ConfigureDialog::gkAddressEdit_textChanged( const QString& text )
{
	configChanged( GK_ADDR, text );
	configChanged( REGISTER_WITH_GK, useGkCheckBox->isChecked(), true );
}

void ConfigureDialog::gkIdEdit_textChanged( const QString& text )
{
	configChanged( GK_ID, text );
	configChanged( REGISTER_WITH_GK, useGkCheckBox->isChecked(), true );
}

void ConfigureDialog::useDiversionCheckBox_toggled( bool b )
{
	configChanged( USE_GK_DIVERSION, b );
}

/* ------------------ general section --------------------------- */

void ConfigureDialog::forwardHostEdit_textChanged( const QString& text )
{
	configChanged( FORWARD_HOST, text );
	configChanged( USE_GK_DIVERSION, useDiversionCheckBox->isChecked(), true );
}


void ConfigureDialog::busyCheckBox_toggled( bool b )
{
	configChanged( BUSY_FORWARD, b );
	configChanged( USE_GK_DIVERSION, useDiversionCheckBox->isChecked(), true );
}


void ConfigureDialog::noAnswerCheckBox_toggled( bool b )
{
	configChanged( NO_ANSWER_FORWARD, b );
	configChanged( USE_GK_DIVERSION, useDiversionCheckBox->isChecked(), true );
}


void ConfigureDialog::alwaysCheckBox_toggled( bool b )
{
	configChanged( ALWAYS_FORWARD, b );
	configChanged( USE_GK_DIVERSION, useDiversionCheckBox->isChecked(), true );
}


void ConfigureDialog::dndCheckBox_toggled( bool b )
{
	configChanged( DO_NOT_DISTURB, b );
}


void ConfigureDialog::autoAnswerCheckBox_toggled( bool b )
{
	configChanged( AUTO_ANSWER, b );
}


void ConfigureDialog::answerSpinBox_valueChanged( int i )
{
	configChanged( ANSWER_TIMEOUT, i );
}


void ConfigureDialog::sendVideoCheckBox_toggled( bool b )
{
	configChanged( SEND_VIDEO, b );
}


void ConfigureDialog::receiveVideoCheckBox_toggled( bool b )
{
	configChanged( RECEIVE_VIDEO, b );
}


/**
 * Show the dialog. Sets values of all items by setValues().
 */
void ConfigureDialog::show()
{
	setupValues();
	QDialog::show();
}

/**
 * Audio configuration widget is inserted from audio plugin.
 * Calls PluginFactory::getPlugin().
 */
void ConfigureDialog::insertAudioConfig()
{
	AudioPlugin *audioPlugin = (AudioPlugin*)pluginFactory->getPlugin(Plugin::Audio);
	ConfigWidget *audioWidget = 0;
	if( audioPlugin ) {
		audioWidget = audioPlugin->createCodecsConfig(audioGroupBox);
		if( audioWidget ) {
			audioGroupBoxLayout->addWidget(audioWidget);
			connect( audioWidget, SIGNAL( changed() ), this, SLOT( setChanged() ) );
			connect( this, SIGNAL( onApplyChanges() ), audioWidget, SLOT( apply() ) );
			connect( this, SIGNAL( onRejectChanges() ), audioWidget, SLOT( reset() ) );
		}
	}
}

/**
 * Video configuration widget is inserted from video-in plugin.
 * Calls PluginFactory::getPlugin().
 */
void ConfigureDialog::insertVideoConfig()
{
	VideoInPlugin *videoPlugin = (VideoInPlugin*)pluginFactory->getPlugin(Plugin::VideoIn);
	ConfigWidget *videoWidget = 0;
	if( videoPlugin ) {
		//we don't need to create videoWidgetLayout, it is already created...
		videoWidget = videoPlugin->createCodecsConfig(videoGroupBox);
		if( videoWidget ) {
			videoGroupBoxLayout->addWidget(videoWidget);
			connect( videoWidget, SIGNAL( changed() ), this, SLOT( setChanged() ) );
			connect( this, SIGNAL( onApplyChanges() ), videoWidget, SLOT( apply() ) );
			connect( this, SIGNAL( onRejectChanges() ), videoWidget, SLOT( reset() ) );
		}
	}
}

/**
 * Get personal information from LDAP server
 * @param map map where to store fetched information
 */
void ConfigureDialog::getPersonal( QMap<QString, QString>& map )
{
	QString dn;
#ifdef ENTERPRISE
	dn = (const char *)endPoint->ldapAttrs.GetString("commOwner");
#endif

	QString server = ancaConf->readEntry( ADMIN_GROUP, "LDAPServer", "localhost" );
	int port = ancaConf->readIntEntry( ADMIN_GROUP, "LDAPPort", 389 );

	PLDAPSession *ldap = new PLDAPSession( /*const PString& defaultBaseDN */ );
	PLDAPSession::SearchContext cnt;
	PStringList attrs;
	attrs.AppendString("cn");
	attrs.AppendString("givenName");
	attrs.AppendString("sn");
	attrs.AppendString("mail");
	attrs.AppendString("telephoneNumber");
	attrs.AppendString("mobile");
	attrs.AppendString("street");
	attrs.AppendString("postalAddress");
	attrs.AppendString("postalCode");
	attrs.AppendString("l");
	attrs.AppendString("o");
	attrs.AppendString("employeeNumber");
	attrs.AppendString("employeeType");
	attrs.AppendString("departmentNumber");
	QStringList data;

	if( !ldap->Open( server.latin1(), port ) ) {
		PTRACE( 1, "Couldn't open connection to LDAP server: " << ldap->GetErrorText() );
		goto out_delete;
	}
	else
		PTRACE( 6, "Connection to LDAP server open.");

	if( !ldap->Search( cnt, "objectClass=*", attrs, dn.latin1()/*, PLDAPSession::ScopeSubTree*/ ) ) {
		PTRACE( 1, "Search query failed: " << ldap->GetErrorText() );
		goto out_close;
	}
	else
		PTRACE( 6, "Query successful.");

	PTRACE( 6, "\nResult:");
	d->existingAttrs.clear();
	for( int i=0; i < attrs.GetSize(); i++ ) {
		PString attr = attrs[i];
		PString result;
		if( ldap->GetSearchResult( cnt, attr, result ) ) {
			d->existingAttrs.append((const char *)attr);
			map[(const char *)attr] = (const char *)result;
			PTRACE( 6, attr << ": " << result );
		}
	}

out_close:
	if( !ldap->Close() ) {
		PTRACE( 1, "Couldn't close connection: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection closed");

out_delete:
	delete ldap;
}

/**
 * Store personal information set in map \p map on LDAP server.
 * @param map personal information to store
 */
void ConfigureDialog::setPersonal( const QMap<QString, QString> map )
{
	QString dn;
#ifdef ENTERPRISE
	dn = (const char *)endPoint->ldapAttrs.GetString("commOwner");
#endif

	QString server = ancaConf->readEntry( ADMIN_GROUP, "LDAPServer", "localhost" );
	int port = ancaConf->readIntEntry( ADMIN_GROUP, "LDAPPort", 389 );
	QString base = ancaConf->readEntry( ADMIN_GROUP, "LDAPPeopleSearchBase", "" );

	PLDAPSession *ldap = new PLDAPSession( /*const PString& defaultBaseDN */ );
	PLDAPSession::SearchContext cnt;
	PList<PLDAPSession::ModAttrib> attrs;

	QMap<QString,QString>::ConstIterator it;
	// Set up what and how we will modify
	for( it = map.begin(); it != map.end(); ++it ) {
		if( it.data().isEmpty() && !d->existingAttrs.contains(it.key()) ) continue;

		PLDAPSession::ModAttrib::Operation op;
		// value is not set and attribute is set in LDAP -> delete it
		if( it.data().isEmpty() )
			op = PLDAPSession::ModAttrib::Delete;
		// value is set and attribute is set in LDAP -> replace it
		else if( d->existingAttrs.contains(it.key()) )
			op = PLDAPSession::ModAttrib::Replace;
		// value is set and attribute is not set in LDAP -> add it
		else
			op = PLDAPSession::ModAttrib::Add;

		if( op == PLDAPSession::ModAttrib::Delete )
			attrs.Append( new PLDAPSession::StringModAttrib(it.key().latin1(), op) );
		else
			attrs.Append( new PLDAPSession::StringModAttrib(it.key().latin1(), it.data().latin1(), op) );
	}
	QString passwd;

	if( !ldap->Open( server.latin1(), port ) ) {
		PTRACE( 1, "Couldn't open connection to LDAP server: " << ldap->GetErrorText() );
		goto out_delete;
	}
	else
		PTRACE( 6, "Connection to LDAP server open.");

	passwd = QInputDialog::getText("Anca", "Enter your LDAP password:", QLineEdit::Password, QString::null, 0, this );
	if( !ldap->Bind( dn.latin1(), passwd.latin1() ) ) {
		PTRACE( 1, "Couldn't bind to LDAP server: " << ldap->GetErrorText() );
		goto out_close;
	}
	else
		PTRACE( 6, "Bound to LDAP server.");

	if( !ldap->Modify( dn.latin1(), attrs ) ) {
		PTRACE( 1, "Modification of LDAP entry failed: " << ldap->GetErrorText() );
		goto out_close;
	}
	else
		PTRACE( 6, "Modification successful.");

out_close:
	if( !ldap->Close() ) {
		PTRACE( 1, "Couldn't close connection: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection closed");

out_delete:
	delete ldap;

}

void ConfigureDialog::moreButton_clicked()
{
	QMap<QString, QString> map;
	getPersonal( map );

	PersonalDialog dlg(this);

	dlg.nickEdit->setText( map["cn"] );
	dlg.firstNameEdit->setText( map["givenName"] );
	dlg.lastNameEdit->setText( map["sn"] );
	dlg.mailEdit->setText( map["mail"] );
	dlg.phoneEdit->setText( map["telephoneNumber"] );
	dlg.mobileEdit->setText( map["mobile"] );
	dlg.streetEdit->setText( map["street"] );
	dlg.cityEdit->setText( map["postalAddress"] );
	dlg.codeEdit->setText( map["postalCode"] );
	dlg.stateEdit->setText( map["l"] );
	dlg.orgEdit->setText( map["o"] );
	dlg.posEdit->setText( map["employeeNumber"] );
	dlg.typeEdit->setText( map["employeeType"] );
	dlg.roomEdit->setText( map["departmentNumber"] );

	if( dlg.exec() == QDialog::Accepted ) {
		map["cn"] = dlg.nickEdit->text();
		map["givenName"] = dlg.firstNameEdit->text();
		map["sn"] = dlg.lastNameEdit->text();
		map["mail"] = dlg.mailEdit->text();
		map["telephoneNumber"] = dlg.phoneEdit->text();
		map["mobile"] = dlg.mobileEdit->text();
		map["street"] = dlg.streetEdit->text();
		map["postalAddress"] = dlg.cityEdit->text();
		map["postalCode"] = dlg.codeEdit->text();
		map["l"] = dlg.stateEdit->text();
		map["o"] = dlg.orgEdit->text();
		map["employeeNumber"] = dlg.posEdit->text();
		map["employeeType"] = dlg.typeEdit->text();
		map["departmentNumber"] = dlg.roomEdit->text();

		// push updated information to LDAP directory
		setPersonal( map );

		// fetch it from it again...
		updatePersonalTab();
	}
}

/**
 * Set personal tab according to information fetched from LDAP server
 * by getPersonal().
 */
void ConfigureDialog::updatePersonalTab()
{
	QMap<QString, QString> map;
	getPersonal( map );
	nickEdit->setText(map["cn"]);
	nameEdit->setText(map["givenName"]);
	surnameEdit->setText(map["sn"]);

	d->personalChanged = false;
}

/**
 * Store information from personal tab on LDAP server by setPersonal().
 */
void ConfigureDialog::setPersonalTab()
{
	QMap<QString, QString> map;
	getPersonal( map );

	map["cn"] = nickEdit->text();
	map["givenName"] = nameEdit->text();
	map["sn"] = surnameEdit->text();

	setPersonal( map );

	d->personalChanged = false;
}

/**
 * Personal tab has changed. Enable apply button.
 */
void ConfigureDialog::setPersonalTabChanged() {
	d->personalChanged = true;
	applyButton->setEnabled(true);
}
