
/*
 *  MOD -- Messenger On-the-Drive
 *
 *  Copyright (C) 2006 Kai-Fong Chou - <mod.mapleelpam at gmail.com>
 *
 *  http://mod.0rz.net
 *  http://mod.sf.net
 *
 */

#include <stdlib.h>
#include <time.h>

#include <qvariant.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qfiledialog.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qinputdialog.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qradiobutton.h>

#include "DrtaMW2.h"
#include "Settings.h"
#include "LogViewer.h"
#include "Error.h"

#include "SctpSocketHandler.h"

#include "dlg_sctp.h"

/* Multi IP supports */

void DrtaMW2 :: slotIPAdd()
{
	try{
		if (hostEdit->currentText().isEmpty())
			throw Error(tr("No host specified!"));
		lb_ips -> insertItem( hostEdit->currentText() );
		hostEdit->clear();
	}
	catch (Error e){
		abortAll(e.getText());
	}
}

void DrtaMW2 :: slotIPDel()
{
	int cur = 0;
	try{
		cur = lb_ips -> currentItem();
		if( cur != -1){
			lb_ips -> removeItem( cur );
		}else{
			//QWarnning?
			debug("no item select");
		}
	}
	catch (Error e){
		abortAll(e.getText());
	}
}
void DrtaMW2 :: slotRecvFrom(QString s)
{
	unsigned int i = 0;
	bool hit = false;

	int cur = lb_recv_mtr -> currentItem();

	for( i = 0 ; i < lb_recv_mtr -> count() ; i++ ){
		if( lb_recv_mtr -> item( i ) -> text() == s){
			hit = true;
			lb_recv_mtr -> changeItem( QPixmap::fromMimeSource( "little_green.png" ) , lb_recv_mtr -> item( i ) -> text() , i  );
			peer_addr_lst . at(i) -> setRecv(true);
		}else{
			lb_recv_mtr -> changeItem( QPixmap::fromMimeSource( "little_gray.png" ) , lb_recv_mtr -> item( i ) -> text() , i  );
			peer_addr_lst . at(i) -> setRecv(false);
		}
	} 

	lb_recv_mtr -> setCurrentItem(cur);
	/*
	if( !hit ){ 
		lb_recv_mtr -> insertItem( QPixmap::fromMimeSource( "little_green.png" ) ,  s);
	}
	*/

}
void DrtaMW2 :: slotSetSctp()
{
	int i = 0;
	if(transmitter ){
		dlg_sctp dlg;
		try{
			dlg . sb_rto_max -> setValue( config.readNumEntry("/drta/sctp/rto_max") );
			dlg . sb_rto_min -> setValue( config.readNumEntry("/drta/sctp/rto_min") );

			if( 0 == config.readNumEntry("/drta/sctp/ttl") )
				dlg . cb_pr -> setChecked( false );

			dlg . sb_ttl -> setValue(  config.readNumEntry("/drta/sctp/ttl") );
			dlg . cb_allow_auto_ch -> setChecked( allow_auto_ho);

			dlg . sb_err_threshold -> setValue( config.readNumEntry("/data/sctp/err_thr"));

			switch ( config.readNumEntry("/data/sctp/ho_policy") ){
				case DRTA_USE_SCTP_POLICY_RP:
					dlg . rb_use_def_rp -> setChecked( true );
					break;
				case DRTA_USE_SCTP_POLICY_POLL:
					dlg . rb_use_poll -> setChecked( true );
					break; 
			};

			for(i = 0 ; i < peer_addr_lst . count() ; i ++){
				dlg . cb_def_rp -> insertItem( peer_addr_lst . at(i) -> getAddr() );
			}
			if( reliable_path >=0 && reliable_path < peer_addr_lst . count() )
				dlg . cb_def_rp -> setCurrentItem( reliable_path );

		}catch(Error e){
			//do nothing
		}
		
		
		if( dlg . exec() == QDialog::Accepted){
			config . writeEntry("/drta/sctp/rto_max",dlg . sb_rto_max -> value());
			config . writeEntry("/drta/sctp/rto_min",dlg . sb_rto_min -> value());

			if( dlg . cb_pr -> isChecked() ){
				config . writeEntry("/drta/sctp/ttl",dlg.sb_ttl->value()); 
			}else{
				config . writeEntry("/drta/sctp/ttl",0); 
			}
			transmitter -> setTTL( config.readNumEntry("/drta/sctp/ttl") ); 

			allow_auto_ho = dlg.cb_allow_auto_ch -> isChecked();
			config . writeEntry("/drta/sctp/allow_auto", allow_auto_ho);	

			error_threshold = dlg . sb_err_threshold -> value();
			config . writeEntry("/data/sctp/err_thr" , error_threshold );

			if( dlg . rb_use_def_rp -> isChecked() ){
				config . writeEntry("/data/sctp/ho_policy" , DRTA_USE_SCTP_POLICY_RP ); 
				hand_over_policy = DRTA_USE_SCTP_POLICY_RP;
			}else{
				config . writeEntry("/data/sctp/ho_policy" , DRTA_USE_SCTP_POLICY_POLL ); 
				hand_over_policy = DRTA_USE_SCTP_POLICY_POLL;
			}
			reliable_path = dlg . cb_def_rp -> currentItem();
		}
	}
}

void DrtaMW2 ::slotSctpEvent(QString str)
{
	lb_sctp_mtr -> insertItem( str); 
}

void DrtaMW2 :: SctpRefreshRemoteIP( int socket )
{

	struct sockaddr *addr_list;
	struct sockaddr_in *in;

	peer_addr_lst . clear();

	// TODO: assoc_id ignore in one-one
	int sock_num = sctp_getpaddrs(socket , 0 , &addr_list );

	lb_recv_mtr -> clear();

	if( sock_num > 0 ){
		int assid = SctpSocketHandler::SctpGetAssocID( transmitter -> getSocketFd() );
		QString prim_str = SctpSocketHandler::SctpGetPrim( transmitter -> getSocketFd(), assid );

		for(int i = 0 ; i < sock_num ; i ++ ){
			in = (struct sockaddr_in*)& addr_list[i];

			char *ip = inet_ntoa( in->sin_addr );
			QString ip_from(ip);

			lb_recv_mtr -> insertItem( ip_from );

			if(prim_str == ip_from)
				peer_addr_lst . append( new PEER_ADDR(ip_from , true) );
			else
				peer_addr_lst . append( new PEER_ADDR(ip_from , false) );
		}

		sctp_freepaddrs( addr_list );
	}else{

	}

}
void DrtaMW2 :: slotSetPrim()
{
	if(transmitter){
		int sd = transmitter -> getSocketFd();
		int index = lb_recv_mtr -> currentItem();

		if(index < 0)	return;
		SctpSocketHandler::SctpSetPrim( sd , index );
//		SctpSocketHandler::SctpSetPeerPrim( sd , lb_recv_mtr -> currentItem() );

		for( unsigned int i = 0 ; i < peer_addr_lst . count() ; i++){
			debug("set %d not prim",i);
			peer_addr_lst . at(index) -> setPrim(false); 
		}
		peer_addr_lst . at(index) -> setPrim(true); 
		debug("new primary = %d",index);
	}
}
void DrtaMW2 :: slotSctpClear()
{
	lb_sctp_mtr -> clear(); 

}

// renyang - 與sigPrSendFailed()相連
void DrtaMW2 :: slotPrSendFailed()
{
	static int error_handled = 0;
	unsigned int new_prim = 0;


	if(!transmitter || !allow_auto_ho){
		return; //or throw some error?
	}

	int sd = transmitter -> getSocketFd();

	// todo make some global value
	if( error_handled ++ > error_threshold ){

		debug(" peer_addr_lst count = %d " , peer_addr_lst.count());
		if(hand_over_policy == DRTA_USE_SCTP_POLICY_MUTE){
			disableIn(); 

			QString msg =tr("Drta: too many failed! try to mute the mic") ;
			slotSctpEvent( msg );
			message( msg);
			QMessageBox::warning(0, "DRTA", msg);
			return;
		}else if(hand_over_policy == DRTA_USE_SCTP_POLICY_POLL ){
			for(new_prim = 0 ; new_prim < peer_addr_lst . count() ; new_prim ++){
				if(peer_addr_lst . at(new_prim) -> getPrim() ){
					debug("org new pramiry = %d?",new_prim);
					new_prim ++;
					break;
				}else{
					debug("not %d",new_prim);
				}
			}
			if(new_prim >= peer_addr_lst . count() )
				new_prim = 0;
		}else if(hand_over_policy == DRTA_USE_SCTP_POLICY_RP ){
			new_prim = reliable_path;
		}

		debug("try to set new primary = %d" , new_prim);
		
		// renyang - 嘗試要切換primary address
		SctpSocketHandler::SctpSetPrim( sd , new_prim); 
		error_handled = 0;

		QString msg =tr("Drta: try to set new primary = ") + peer_addr_lst . at(new_prim) ->  getAddr();
		slotSctpEvent( msg );
		message( msg);
		QMessageBox::warning(0, "DRTA", msg);
	}
}
