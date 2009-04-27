/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

#include "ancainfo.h"
#include "ancaconf.h"
#include "stdinfo.h"
#include "listview.h"
#include "editdialog.h"

#include <qheader.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>

#include <ptlib.h>
#include <ptclib/pldap.h>
#include <ldap.h>

#define DELETE_COLUMN 2
#define EDIT_COLUMN 3

typedef QPtrList<ListViewItem> ListViewItemList;
/**
 * TAH = TopTen, All, History, Search
 */
class TAHField
{
public:
	TAHField(): m_topten(0), m_all(0), m_search(0) { m_history.setAutoDelete(true); }
	~TAHField() {}

	void addHistory( ListViewItem *historyItem ) { m_history.append(historyItem); }
	void removeHistory( ListViewItem *historyItem ) { m_history.removeRef(historyItem); }
	void clearHistory() { m_history.clear(); }
	void setAll( ListViewItem *allItem ) { m_all = allItem; }
	void setTopTen( ListViewItem *toptenItem ) { m_topten = toptenItem; }
	void setSearch( ListViewItem *searchItem ) { m_search = searchItem; }

	ListViewItemList history() { return m_history; }
	ListViewItem *all() { return m_all; }
	ListViewItem *topten() { return m_topten; }
	ListViewItem *search() { return m_search; }

	bool allEmpty() { return !m_topten && !m_all && !m_search && m_history.isEmpty(); }

private:
	ListViewItem *m_topten;
	ListViewItem *m_all;
	ListViewItem *m_search;
	ListViewItemList m_history;
};

class AddressBookWidgetPrivate
{
public:
	enum CurrentListView { TopTen, All, History, Search };

	AddressBookWidgetPrivate( AddressBookWidget *adw )
		: aItem(0),
		  dItem(0),
		  gItem(0),
		  jItem(0),
		  mItem(0),
		  pItem(0),
		  sItem(0),
		  vItem(0),
		  adb(0),
		  topTenLastSelected(0),
		  allLastSelected(0),
		  historyLastSelected(0),
		  searchLastSelected(0),
		  ldap(0),
		  adw(adw)
	{
	}

	~AddressBookWidgetPrivate() {}

	ListViewItem *aItem;
	ListViewItem *dItem;
	ListViewItem *gItem;
	ListViewItem *jItem;
	ListViewItem *mItem;
	ListViewItem *pItem;
	ListViewItem *sItem;
	ListViewItem *vItem;

	AddressBook *adb;

	QListViewItem *topTenLastSelected;
	QListViewItem *allLastSelected;
	QListViewItem *historyLastSelected;
	QListViewItem *searchLastSelected;

	void setLastSelected( CurrentListView clv, QListViewItem *item )
	{
		switch( clv ) {
			case TopTen: topTenLastSelected = item; break;
			case All: allLastSelected = item; break;
			case History: historyLastSelected = item; break;
			case Search: searchLastSelected = item; break;
		}
	}
	void setLastSelected( QListViewItem *item ) { setLastSelected( (CurrentListView)adw->tabWidget->currentPageIndex(), item ); }

	QListViewItem *lastSelected( CurrentListView clv )
	{
		QListViewItem *item = 0;
		switch( clv ) {
			case TopTen: item = topTenLastSelected; break;
			case All: item = allLastSelected; break;
			case History: item = historyLastSelected; break;
			case Search: item = searchLastSelected; break;
		}
		return item;
	}
	QListViewItem *lastSelected() { return lastSelected( (CurrentListView)adw->tabWidget->currentPageIndex()); }

	PLDAPSession *ldap;

private:
	AddressBookWidget *adw;
};

void AddressBookWidget::init()
{
	d = new AddressBookWidgetPrivate(this);

	ancaInfo->connectNotify( ADDRESS_CALLING, this, SLOT( addressCalling( const QVariant& ) ) );
	ancaInfo->connectNotify( CALLER_ADDRESS, this, SLOT( callerAddress( const QVariant& ) ) );

	// columns must be in this order (DELETE_COLUMN=2, EDIT_COLUMN=3)
	topTenListView->addColumn( "fill", 0 );
	topTenListView->addColumn( "contact" );
	topTenListView->addColumn( "delete", 19 );
	topTenListView->addColumn( "edit", 19 );
	topTenListView->header()->hide();
	topTenListView->setResizeMode( QListView::LastColumn );
	topTenListView->header()->setStretchEnabled( true, 1 );
	topTenListView->setSorting( -1 );
	topTenListView->setAllColumnsShowFocus(true);
	topTenListView->setAlternateBackground(QColor(50,50,50));

	allListView->addColumn( "sortingLetter", 19 );
	allListView->addColumn( "contact" );
	allListView->addColumn( "delete", 19 );
	allListView->addColumn( "edit", 19 );
	allListView->header()->hide();
	allListView->setResizeMode( QListView::LastColumn );
	allListView->header()->setStretchEnabled( true, 1 );
	allListView->setSorting(1);
	allListView->setAllColumnsShowFocus(true);
	allListView->setAlternateBackground(QColor(50,50,50));

	historyListView->addColumn( "type", 19 );
	historyListView->addColumn( "contact" );
	historyListView->addColumn( "delete", 19 );
	historyListView->addColumn( "edit", 19 );
	historyListView->header()->hide();
	historyListView->setResizeMode( QListView::LastColumn );
	historyListView->header()->setStretchEnabled( true, 1 );
	historyListView->setSorting( -1 );
	historyListView->setAllColumnsShowFocus(true);
	historyListView->setAlternateBackground(QColor(50,50,50));

	resultListView->addColumn( "fill", 0 );
	resultListView->addColumn( "contact" );
	resultListView->addColumn( "delete", 19 );
	resultListView->addColumn( "edit", 19 );
	resultListView->header()->hide();
	resultListView->setResizeMode( QListView::LastColumn );
	resultListView->header()->setStretchEnabled( true, 1 );
	resultListView->setSorting( -1 );
	resultListView->setAllColumnsShowFocus(true);
	resultListView->setAlternateBackground(QColor(50,50,50));

	connect( allListView, SIGNAL( clicked( QListViewItem *, const QPoint&, int )), this, SLOT( listViewClicked( QListViewItem *, const QPoint&, int )) );
	connect( topTenListView, SIGNAL( clicked( QListViewItem *, const QPoint&, int )), this, SLOT( listViewClicked( QListViewItem *, const QPoint&, int )) );
	connect( historyListView, SIGNAL( clicked( QListViewItem *, const QPoint&, int )), this, SLOT( listViewClicked( QListViewItem *, const QPoint&, int )) );
	connect( resultListView, SIGNAL( clicked( QListViewItem *, const QPoint&, int )), this, SLOT( listViewClicked( QListViewItem *, const QPoint&, int )) );
	connect( topTenListView, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( currentAddressChanged( QListViewItem * ) ) );
	connect( allListView, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( currentAddressChanged( QListViewItem * ) ) );
	connect( historyListView, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( currentAddressChanged( QListViewItem * ) ) );
	connect( resultListView, SIGNAL( clicked( QListViewItem * ) ), this, SLOT( currentAddressChanged( QListViewItem * ) ) );

#ifndef Q_WS_QWS
	connect( allListView, SIGNAL( doubleClicked( QListViewItem *, const QPoint&, int )), this, SLOT( call( QListViewItem *, const QPoint&, int )) );
	connect( topTenListView, SIGNAL( doubleClicked( QListViewItem *, const QPoint&, int )), this, SLOT( call( QListViewItem *, const QPoint&, int )) );
	connect( historyListView, SIGNAL( doubleClicked( QListViewItem *, const QPoint&, int )), this, SLOT( call( QListViewItem *, const QPoint&, int )) );
	connect( resultListView, SIGNAL( doubleClicked( QListViewItem *, const QPoint&, int )), this, SLOT( call( QListViewItem *, const QPoint&, int )) );
#endif

	aButton->setEnabled(false);
	dButton->setEnabled(false);
	gButton->setEnabled(false);
	jButton->setEnabled(false);
	mButton->setEnabled(false);
	pButton->setEnabled(false);
	sButton->setEnabled(false);
	vButton->setEnabled(false);

	d->ldap = new PLDAPSession( /*const PString& defaultBaseDN */ );
}

void AddressBookWidget::destroy()
{
	delete d->ldap;
	delete d;
}


void AddressBookWidget::currentAddressChanged( QListViewItem *listViewItem )
{
	if( !listViewItem )
		return;
	
	AdbItem *item = ((ListViewItem*)listViewItem)->getAdbItem();
	if( !item ) return;
	ancaInfo->set( ADDRESS_TO_CALL, item->url() );

	if( (AddressBookWidgetPrivate::CurrentListView)tabWidget->currentPageIndex() == AddressBookWidgetPrivate::All )
		listViewItem->setPixmap(2, QPixmap::fromMimeSource("delete.png"));
	if( item->bookmarked() )
		listViewItem->setPixmap(3, QPixmap::fromMimeSource("edit.png"));
	else
		listViewItem->setPixmap(3, QPixmap::fromMimeSource("save.png"));
	if( d->lastSelected() && d->lastSelected() != listViewItem && d->lastSelected()->pixmap(3) ) {
		d->lastSelected()->setPixmap(2,QPixmap());
		d->lastSelected()->setPixmap(3,QPixmap());
	}
	d->setLastSelected(listViewItem);
}


void AddressBookWidget::addressCalling( const QVariant & url )
{
	AdbItem *item = d->adb->insert( url.toString(), false );
	ancaInfo->set( PARTY_PICTURE, item->picture() );
}

void AddressBookWidget::callerAddress( const QVariant & url )
{
	AdbItem *item = d->adb->insert( url.toString(), true );
	ancaInfo->set( PARTY_PICTURE, item->picture() );
}


void AddressBookWidget::setAddressBook( AddressBook *adb )
{
	d->adb = adb;
	
	connect( adb, SIGNAL(updateTopTenWidget( AdbItemList * )), this, SLOT(updateTopTen(AdbItemList*)) );
	connect( adb, SIGNAL(updateHistoryWidget( AdbItem *, AdbItem *, bool)), this, SLOT(updateHistory(AdbItem*, AdbItem*, bool)) );

	connect( adb, SIGNAL(fillHistoryWidget( HistoryQueue *)), this, SLOT(fillHistory(HistoryQueue*)) );
	connect( adb, SIGNAL(fillAllWidget( AdbItemDirectory*)), this, SLOT(fillAllView(AdbItemDirectory*)) );

	connect( adb, SIGNAL(destroyIt(AdbItem*)), this, SLOT(removeViewItem(AdbItem*)));
	connect( adb, SIGNAL(unbookmarkIt(AdbItem*)), this, SLOT(removeViewItemFromAll(AdbItem*)));

	connect( adb, SIGNAL(edited(AdbItem*)), this, SLOT(itemChanged(AdbItem*)));
	connect( adb, SIGNAL(bookmarked(AdbItem*)), this, SLOT(itemSaved(AdbItem*)));
}


void AddressBookWidget::updateTopTen( AdbItemList *topten )
{
	//this is stupid algorithm
	//TODO do it better
	
	//clear topTenListView, but before doing it clear references in the AdbItems
	for( QListViewItem *item = topTenListView->firstChild(); item; item = item->nextSibling() ) {
		AdbItem *adbItem = ((ListViewItem*)item)->getAdbItem();
		if( adbItem ) {
			TAHField *f = (TAHField*)adbItem->customData();
			if( !f ) continue;
			f->setTopTen(0);
			if(f->allEmpty()) {
				delete f;
				adbItem->setCustomData(0);
			}
		}
	}
	topTenListView->clear();
	
	ListViewItem *last = 0;
	for( AdbItem *item = topten->first(); item; item = topten->next() ) {
		if( !item->customData() ) item->setCustomData(new TAHField());
		ListViewItem *listViewItem = new ListViewItem( topTenListView, item, 1, last );
		last = listViewItem;
		((TAHField*)item->customData())->setTopTen(listViewItem);
	}

	d->setLastSelected(AddressBookWidgetPrivate::TopTen, 0);
}


void AddressBookWidget::updateHistory( AdbItem *toRemove, AdbItem *toInsert, bool incoming)
{
	if( toRemove ) {
		ListViewItem *item = (ListViewItem*)historyListView->lastItem();
		TAHField *f = (TAHField*)toRemove->customData();
		Q_ASSERT( f && f->history().findRef(item) );

		if( f ) {
			if( item == d->lastSelected(AddressBookWidgetPrivate::History) )
				d->setLastSelected(AddressBookWidgetPrivate::History, 0 );
			f->removeHistory(item);
			if(f->allEmpty()) {
				delete f;
				toRemove->setCustomData(0);
			}
		}
		else
			//here it shouldn't never come
			delete item;
	}

	if( toInsert ) {
		//Insert appropriate listViewItem to the historyListView (to the beginning)
		if( !toInsert->customData() ) toInsert->setCustomData(new TAHField());
		ListViewItem *listViewItem = new HistoryItem( historyListView, toInsert, 1, 0, incoming, 0 );
		((TAHField*)toInsert->customData())->addHistory(listViewItem);
	}
}

void AddressBookWidget::fillHistory( HistoryQueue *history )
{
	for( QValueList<QPair<AdbItem *, bool> >::iterator it = history->begin(); it != history->end(); ++it ) {
		AdbItem *item = (*it).first;
		bool incoming = (*it).second;
		if( !item->customData() ) item->setCustomData(new TAHField());
		ListViewItem *listViewItem = new HistoryItem( historyListView, item, 1, 0, incoming );
		((TAHField*)item->customData())->addHistory(listViewItem);
	}
}

void AddressBookWidget::fillAllView( AdbItemDirectory *dir )
{
	for( AdbItemDirectoryIterator it(*dir); it.current(); ++it ) {
		AdbItem *item = it.current();
		if( !item->bookmarked() ) continue;
		if( !item->customData() ) item->setCustomData(new TAHField());
		ListViewItem *listViewItem = new ListViewItem( allListView, item, 1 );
		checkHelpers(listViewItem->text(1));
		((TAHField*)item->customData())->setAll(listViewItem);
	}
}

void AddressBookWidget::removeViewItem( AdbItem *item )
{
	if( !item ) return;

	TAHField *f = (TAHField*)item->customData();
	if( !f ) return;

	if( !f->history().isEmpty() ) {
		if( f->history().findRef((ListViewItem*)d->lastSelected(AddressBookWidgetPrivate::History)) )
			d->setLastSelected(AddressBookWidgetPrivate::History, 0 );
		f->clearHistory();
	}
	if( f->topten() ) {
		if( f->topten() == d->lastSelected(AddressBookWidgetPrivate::TopTen) )
			d->setLastSelected(AddressBookWidgetPrivate::TopTen, 0);
		delete f->topten();
	}
	if( f->all() ) {
		if( f->all() == d->lastSelected(AddressBookWidgetPrivate::All) )
			d->setLastSelected(AddressBookWidgetPrivate::All, 0);
		delete f->all();
	}
	if( f->search() ) {
		if( f->search() == d->lastSelected(AddressBookWidgetPrivate::Search) )
			d->setLastSelected(AddressBookWidgetPrivate::Search, 0);
		delete f->search();
	}
	delete f;
	item->setCustomData(0);

}

void AddressBookWidget::removeViewItemFromAll( AdbItem *item )
{
	if( !item ) return;

	TAHField *f = (TAHField*)item->customData();
	if( !f ) return;

	if( f->all() ) {
		if( f->all() == d->lastSelected(AddressBookWidgetPrivate::All) )
			d->setLastSelected(AddressBookWidgetPrivate::All, 0);
		delete f->all();
		if( f->history().isEmpty() && !f->topten() ) {
			delete f;
			item->setCustomData(0);
		}
	}

}

void AddressBookWidget::aButton_clicked()
{
	allListView->setCurrentItem(d->aItem);
	allListView->ensureItemVisible(d->aItem);
}

void AddressBookWidget::dButton_clicked()
{
	allListView->setCurrentItem(d->dItem);
	allListView->ensureItemVisible(d->dItem);
}

void AddressBookWidget::gButton_clicked()
{
	allListView->setCurrentItem(d->gItem);
	allListView->ensureItemVisible(d->gItem);
}

void AddressBookWidget::jButton_clicked()
{
	allListView->setCurrentItem(d->jItem);
	allListView->ensureItemVisible(d->jItem);
}

void AddressBookWidget::mButton_clicked()
{
	allListView->setCurrentItem(d->mItem);
	allListView->ensureItemVisible(d->mItem);
}

void AddressBookWidget::pButton_clicked()
{
	allListView->setCurrentItem(d->pItem);
	allListView->ensureItemVisible(d->pItem);
}

void AddressBookWidget::sButton_clicked()
{
	allListView->setCurrentItem(d->sItem);
	allListView->ensureItemVisible(d->sItem);
}

void AddressBookWidget::vButton_clicked()
{
	allListView->setCurrentItem(d->vItem);
	allListView->ensureItemVisible(d->vItem);
}

void AddressBookWidget::listViewClicked( QListViewItem * item, const QPoint&, int c )
{
	ListViewItem *i = (ListViewItem*)item;
	if( !i || !i->getAdbItem() ) return;

	AdbItem *adbItem = i->getAdbItem();

	if( c == DELETE_COLUMN ) {
		if( (AddressBookWidgetPrivate::CurrentListView)tabWidget->currentPageIndex() == AddressBookWidgetPrivate::All ) {
			QMessageBox mb( "Delete contact -- Anca",
					QString("You are going to delete '%1'\n"
						"Do you really want to delete it?").arg(item->text(1)),
					QMessageBox::Information,
					QMessageBox::Yes | QMessageBox::Default,
					QMessageBox::Cancel | QMessageBox::Escape,
					QMessageBox::NoButton );
			mb.setButtonText( QMessageBox::Yes, "&Delete" );

			if( mb.exec() == QMessageBox::Yes )
				d->adb->unbookmarkItem(adbItem);
		}
	}
	else if( c == EDIT_COLUMN ) {
		EditDialog dlg(this);
		dlg.setItem(adbItem);
		if( dlg.exec() == QDialog::Accepted ) {
			d->adb->bookmarkItem(adbItem, 
				adbItem->id(),
				dlg.nickEdit->text(),
				dlg.firstNameEdit->text(),
				dlg.lastNameEdit->text(),
				dlg.mailEdit->text(),
				dlg.phoneEdit->text(),
				dlg.mobileEdit->text(),
				dlg.streetEdit->text(),
				dlg.cityEdit->text(),
				dlg.codeEdit->text(),
				dlg.stateEdit->text(),
				dlg.orgEdit->text(),
				dlg.posEdit->text(),
				dlg.typeEdit->text(),
				dlg.roomEdit->text(),
				dlg.urlEdit->text(),
				dlg.picture() );
		}
	}
}

void AddressBookWidget::call( QListViewItem * item, const QPoint&, int )
{
	ListViewItem *i = (ListViewItem*)item;
	if( !i || !i->getAdbItem() ) return;

	AdbItem *adbItem = i->getAdbItem();
	if( !adbItem ) return;

	ancaInfo->set( ADDRESS_TO_CALL_NOW, adbItem->url() );
}

void AddressBookWidget::itemChanged( AdbItem * adbItem )
{
	if( !adbItem ) return;

	TAHField *f = (TAHField*)adbItem->customData();
	if( !f ) return;

	if( !f->history().isEmpty() ) {
		ListViewItemList list = f->history();
		for( ListViewItem *item = list.first(); item; item = list.next() )
			item->reset();
	}
	if( f->topten() ) f->topten()->reset();
	if( f->all() ) f->all()->reset();
	if( f->search() ) f->search()->reset();
}

void AddressBookWidget::itemSaved( AdbItem *item )
{
	ListViewItem *listViewItem = new ListViewItem( allListView, item, 1 );
	checkHelpers(listViewItem->text(1));
	
	TAHField *f = (TAHField*)item->customData();
	if( !f ) item->setCustomData(f = new TAHField());
	f->setAll(listViewItem);
	
	//rename existing listViewItems with the same adbItem
	if( !f->history().isEmpty() ) {
		ListViewItemList list = f->history();
		for( ListViewItem *item = list.first(); item; item = list.next() )
			item->reset();
	}
	if( f->topten() ) f->topten()->reset();
	if( f->search() ) f->search()->reset();
}


void AddressBookWidget::newContactButton_clicked()
{

	EditDialog dlg(this);
	dlg.creatingNewContact();
	if( dlg.exec() == QDialog::Accepted ) {
		AdbItem *item = d->adb->insert(
			QString::null,
			dlg.nickEdit->text(),
			dlg.firstNameEdit->text(),
			dlg.lastNameEdit->text(),
			dlg.mailEdit->text(),
			dlg.phoneEdit->text(),
			dlg.mobileEdit->text(),
			dlg.streetEdit->text(),
			dlg.cityEdit->text(),
			dlg.codeEdit->text(),
			dlg.stateEdit->text(),
			dlg.orgEdit->text(),
			dlg.posEdit->text(),
			dlg.typeEdit->text(),
			dlg.roomEdit->text(),
			dlg.urlEdit->text(),
			dlg.picture() );
		item->setCustomData(new TAHField());
		ListViewItem *listViewItem = new ListViewItem( allListView, item, 1 );
		checkHelpers(listViewItem->text(1));
		((TAHField*)item->customData())->setAll(listViewItem);
	}
}

void AddressBookWidget::checkHelpers( const QString& s )
{
	static const char *table[8] = { "a", "d", "g", "j", "m", "p", "s", "v" };
	int i = 0;
	
	while( i < 8 ) {
		if( s < QString(table[i]) ) {
			break;
		}
		i++;
	}
	
	if( i == 1 ) {
		if( !d->aItem ) {
			d->aItem = new ListViewItem( allListView, "a", QPixmap::fromMimeSource("a.png") );
			d->aItem->setSelectable(false);
			aButton->setEnabled(true);
		}
	}
	else if( i == 2 ) {
		if( !d->dItem ) {
			d->dItem = new ListViewItem( allListView, "d", QPixmap::fromMimeSource("d.png") );
			d->dItem->setSelectable(false);
			dButton->setEnabled(true);
		}
	}
	else if( i == 3 ) {
		if( !d->gItem ) {
			d->gItem = new ListViewItem( allListView, "g", QPixmap::fromMimeSource("g.png") );
			d->gItem->setSelectable(false);
			gButton->setEnabled(true);
		}
	}
	else if( i == 4 ) {
		if( !d->jItem ) {
			d->jItem = new ListViewItem( allListView, "j", QPixmap::fromMimeSource("j.png") );
			d->jItem->setSelectable(false);
			jButton->setEnabled(true);
		}
	}
	else if( i == 5 ) {
		if( !d->mItem ) {
			d->mItem = new ListViewItem( allListView, "m", QPixmap::fromMimeSource("m.png") );
			d->mItem->setSelectable(false);
			mButton->setEnabled(true);
		}
	}
	else if( i == 6 ) {
		if( !d->pItem ) {
			d->pItem = new ListViewItem( allListView, "p", QPixmap::fromMimeSource("p.png") );
			d->pItem->setSelectable(false);
			pButton->setEnabled(true);
		}
	}
	else if( i == 7 ) {
		if( !d->sItem ) {
			d->sItem = new ListViewItem( allListView, "s", QPixmap::fromMimeSource("s.png") );
			d->sItem->setSelectable(false);
			sButton->setEnabled(true);
		}
	}
	else if( i == 8 ) {
		if( !d->vItem ) {
			d->vItem = new ListViewItem( allListView, "v", QPixmap::fromMimeSource("v.png") );
			d->vItem->setSelectable(false);
			vButton->setEnabled(true);
		}
	}
}

void AddressBookWidget::searchButton_clicked()
{
	for( QListViewItem *item = resultListView->firstChild(); item; item = item->nextSibling() ) {
		AdbItem *adbItem = ((ListViewItem*)item)->getAdbItem();
		if( adbItem ) {
			TAHField *f = (TAHField*)adbItem->customData();
			if( !f ) continue;
			f->setSearch(0);
			if(f->allEmpty()) {
				delete f;
				adbItem->setCustomData(0);
			}
		}
	}
	resultListView->clear();
	d->setLastSelected(AddressBookWidgetPrivate::Search, 0);

	PLDAPSession *ldap = d->ldap;

	QString server = ancaConf->readEntry( ADMIN_GROUP, "LDAPServer", "localhost" );
	int port = ancaConf->readIntEntry( ADMIN_GROUP, "LDAPPort", 389 );
	QString base = ancaConf->readEntry( ADMIN_GROUP, "LDAPPeopleSearchBase", "" );

	int nResults = 0;
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
	attrs.AppendString("uid");
	attrs.AppendString("commURI");
	QString filter;
	QStringList data;

	if( !ldap->IsOpen() && !ldap->Open( server.latin1(), port ) ) {
		PTRACE( 1, "Couldn't open connection to LDAP server: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection to LDAP server open.");

	if( searchEdit->text().isEmpty() )
		filter = QString( "(objectClass=inetOrgPerson)" );
	else
		// search in cn and sn...
		filter = QString( "(&(objectClass=inetOrgPerson)(|(cn=*%1*)(sn=*%2*)))" ).arg( searchEdit->text() ).arg( searchEdit->text() );

	if( !ldap->Search( cnt, filter.latin1(), attrs, base.latin1()/*, PLDAPSession::ScopeSubTree*/ ) ) {
		PTRACE( 1, "Search query failed: " << ldap->GetErrorText() );
		goto out;
	}
	else
		PTRACE( 6, "Query successful.");

	PTRACE( 6, "\nResult:");
	do {
		data.clear();
		for( int i=0; i < attrs.GetSize(); i++ ) {
			PString attr = attrs[i];
			PString result;
			if( ldap->GetSearchResult( cnt, attr, result ) ) {
				data.append( (const char *)result );
				PTRACE( 6, attr << ": " << result );
			}
			else {
				data.append( QString::null );
			}
		}

		data[15] = getAlias(data[15]);

		/* data: cn, givenName, surname, mail, telephoneNumber, mobile, street, postalAddress,
		 * postalCode, l, o, employeeNumber, employeeType, departmentNumber, uid, commURI */
		AdbItem *item = new AdbItem( data[14], data[0], data[1], data[2], data[3], data[4], data[5], data[6],
				data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[15], "default", false );
		ListViewItem *listViewItem = new ListViewItem( resultListView, item, 1 );
		item->setCustomData(new TAHField());
		((TAHField*)item->customData())->setSearch(listViewItem);

		nResults++;
	} while( ldap->GetNextSearchResult(cnt) );
			
out:
	PTRACE( 6,  "Found: " << nResults );

	if( !ldap->Close() ) {
		PTRACE( 1, "Couldn't close connection: " << ldap->GetErrorText() );
		return;
	}
	else
		PTRACE( 6, "Connection closed");
}


QString AddressBookWidget::getAlias( const QString & url )
{
	QString alias;

	if( url.isEmpty() || !ldap_is_ldap_url(url.latin1()) ) return alias;

	PLDAPSession *ldap = new PLDAPSession( /*const PString& defaultBaseDN */ );
	LDAPURLDesc *urlDesc = 0;
	PLDAPSession::SearchContext cnt;
	PString result;
	PStringList attrs;
	attrs.AppendString("h323Identityh323-ID");

	if( ldap_url_parse( url.latin1(), &urlDesc ) != 0 ) goto out_return;

	if( !ldap->IsOpen() && !ldap->Open( urlDesc->lud_host, urlDesc->lud_port ) ) {
		PTRACE( 1, "Couldn't open connection to LDAP server: " << ldap->GetErrorText() );
		goto out_alias;
	}
	else
		PTRACE( 6, "Connection to LDAP server open.");

	if( !ldap->Search( cnt, urlDesc->lud_filter, attrs, urlDesc->lud_dn, (PLDAPSession::SearchScope)urlDesc->lud_scope ) ) {
		PTRACE( 1, "Search query failed: " << ldap->GetErrorText() );
		goto out;
	}
	else
		PTRACE( 6, "Query successful.");

	PTRACE( 6, "\nResult:");
	if( ldap->GetSearchResult( cnt, "h323Identityh323-ID", result ) ) {
		alias = (const char *)result;
		PTRACE( 6, "h323Identityh323-ID: " << result );
	}

out:
	if( !ldap->Close() )
		PTRACE( 1, "Couldn't close connection: " << ldap->GetErrorText() );
	else
		PTRACE( 6, "Connection closed");

out_alias:
	ldap_free_urldesc( urlDesc );

out_return:
	delete ldap;
	return alias;
}
