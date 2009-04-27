#include "addressbook.h"
#include "ancaconf.h"

/**************** AdbItem ********************/

AdbItem::AdbItem( const AdbItem& item )
	: m_inLDAP(item.m_inLDAP), m_nick(item.m_nick), m_givenName(item.m_givenName),
	m_surname(item.m_surname), m_email(item.m_email),
	m_telephone(item.m_telephone), m_mobile(item.m_mobile),
	m_street(item.m_street), m_city(item.m_city), m_code(item.m_code),
	m_state(item.m_state),
	m_organization(item.m_organization),
	m_employeeNumber(item.m_employeeNumber),
	m_employeeType(item.m_employeeType),
	m_departmentNumber(m_departmentNumber), 
	m_id(item.m_id),
	m_url(item.m_url), m_picture(item.m_picture),
	m_in_history(item.m_in_history), m_outcoming(item.m_outcoming),
	m_in_topten(item.m_in_topten), m_bookmarked(item.m_bookmarked),
	m_was_called(item.m_was_called), data(item.data)
{}

AdbItem::AdbItem( const QString& url )
	: m_inLDAP(false), m_url(url), m_picture("default"), m_in_history(0), m_outcoming(0),
	m_in_topten(false), m_bookmarked(false), data(0) 
{}

AdbItem::AdbItem( const QString& id,
	const QString& nick, const QString& givenName, const QString& surname, 
	const QString& email, const QString& telephone, const QString& mobile,
	const QString& street, const QString& city, const QString& code, const QString& state,
	const QString& organization, const QString& employeeNumber, const QString& employeeType, 
	const QString& departmentNumber, const QString& url, const QString& picture, bool book )
	: m_inLDAP(id.isEmpty() ? false : true), m_nick(nick), 
	m_givenName(givenName), m_surname(surname), m_email(email),
	m_telephone(telephone), m_mobile(mobile), m_street(street),
	m_city(city), m_code(code), m_state(state),
	m_organization(organization), m_employeeNumber(employeeNumber),
	m_employeeType(employeeType), m_departmentNumber(departmentNumber),
	m_id(id), m_url(url), m_picture(picture), m_in_history(0), m_outcoming(0),
	m_in_topten(false), m_bookmarked(book), data(0)
{}

AdbItem::~AdbItem()
{}

const QString& AdbItem::nick() const
{
	return m_nick;
}

void AdbItem::setNick( const QString& nick )
{
	m_nick = nick;
}

const QString& AdbItem::givenName() const
{
	return m_givenName;
}

void AdbItem::setGivenName( const QString& givenName )
{
	m_givenName = givenName;
}

const QString& AdbItem::surname() const
{
	return m_surname;
}

void AdbItem::setSurname( const QString& surname )
{
	m_surname = surname;
}

const QString& AdbItem::email() const
{
	return m_email;
}

void AdbItem::setEmail( const QString& email )
{
	m_email = email;
}

const QString& AdbItem::telephone() const
{
	return m_telephone;
}

void AdbItem::setTelephone( const QString& telephone )
{
	m_telephone = telephone;
}

const QString& AdbItem::mobile() const
{
	return m_mobile;
}

void AdbItem::setMobile( const QString& mobile )
{
	m_mobile = mobile;
}

const QString& AdbItem::street() const
{
	return m_street;
}

void AdbItem::setStreet( const QString& street )
{
	m_street = street;
}

const QString& AdbItem::city() const
{
	return m_city;
}

void AdbItem::setCity( const QString& city )
{
	m_city = city;
}

const QString& AdbItem::code() const
{
	return m_code;
}

void AdbItem::setCode( const QString& code )
{
	m_code = code;
}

const QString& AdbItem::state() const
{
	return m_state;
}

void AdbItem::setState( const QString& state )
{
	m_state = state;
}

const QString& AdbItem::organization() const
{
	return m_organization;
}

void AdbItem::setOrganization( const QString& organization )
{
	m_organization = organization;
}

const QString& AdbItem::employeeNumber() const
{
	return m_employeeNumber;
}

void AdbItem::setEmployeeNumber( const QString& employeeNumber )
{
	m_employeeNumber = employeeNumber;
}

const QString& AdbItem::employeeType() const
{
	return m_employeeType;
}

void AdbItem::setEmployeeType( const QString& employeeType )
{
	m_employeeType = employeeType;
}

const QString& AdbItem::departmentNumber() const
{
	return m_departmentNumber;
}

void AdbItem::setDepartmentNumber( const QString& departmentNumber )
{
	m_departmentNumber = departmentNumber;
}

const QString& AdbItem::id() const
{
	return m_id;
}

void AdbItem::setId( const QString& id )
{
	m_id = id;
	m_inLDAP = m_id.isEmpty() ? false : true;
}

const QString& AdbItem::url() const
{
	return m_url;
}

void AdbItem::setURL( const QString& url )
{
	m_url = url;
}

bool AdbItem::inLDAP() const
{
	return m_inLDAP;
}

void AdbItem::setBookmarked( bool set )
{
	m_bookmarked = set;
}

bool AdbItem::bookmarked() const
{
	return m_bookmarked;
}

void AdbItem::setPicture( const QString& picture )
{
	m_picture = picture;
}

const QString& AdbItem::picture() const
{
	return m_picture;
}

void AdbItem::setCustomData( void *data )
{
	this->data = data;
}

void *AdbItem::customData()
{
	return data;
}

int AdbItem::incInHistory(bool outcoming)
{
	if( outcoming ) ++m_outcoming;
	return ++m_in_history;
}

int AdbItem::decInHistory(bool outcoming)
{
	if( outcoming ) ++m_outcoming;
	return --m_in_history;
}

/**************** HistoryQueue ***************/

HistoryQueue::HistoryQueue( unsigned size ): size(size)
{}

HistoryQueue::HistoryQueue( const HistoryQueue& queue ): QValueList<QPair<AdbItem*, bool> >(queue), size(queue.size)
{}

HistoryQueue::~HistoryQueue()
{}

AdbItem *HistoryQueue::insert( AdbItem *item, bool incoming )
{
	AdbItem *toRemove = 0;

	// if the queue is full remove the first item
	if( count() == size ) {
		toRemove = first().first;
		remove(first());
	}

	// insert item to the back of the queue
	append(QPair<AdbItem*,bool>(item,incoming));

	return toRemove;
}

/**************** AdbItemList ***************/

void AdbItemList::sort(int maxSize)
{
	AdbItemList **radix = new AdbItemList*[maxSize+1];

	for( int i = 0; i <= maxSize; i++ ) radix[i] = 0;

	for( AdbItemListIterator it(*this); *it; ++it ) {
		AdbItemList *list = radix[(*it)->m_outcoming];
		if( !list )  list = radix[(*it)->m_outcoming] = new AdbItemList();
		list->append(*it);
	}

	clear();

	for( int i = 0; i <= maxSize; i++ )
		if( radix[i] ) {
			for( AdbItemListIterator it(*radix[i]); *it; ++it )
				prepend(*it);
			delete radix[i];
		}

	delete[] radix;
}

/**************** AddressBook ***************/

AddressBook::AddressBook(): history(100)
{
	dir.setAutoDelete(true);
}

AddressBook::~AddressBook()
{}

AdbItem *AddressBook::insert( const QString& url, bool incoming )
{
	int index = 0;
	AdbItem *item;
	AdbItem *tmp = new AdbItem( url );

	// url doesn't contain any AdbItem
	if( (index = dir.find(tmp)) == -1 ) {
		item = tmp;
		dir.append(tmp);
	}
	else {
		// current is corrently set after find()
		item = dir.current();
		delete tmp;
	}

	// insert item to the history
	AdbItem *removed = history.insert(item, incoming);

	// update in-history field of items
	if( removed ) removed->decInHistory(!incoming);
	item->incInHistory(!incoming);

	// and update topten list
	updateTopTen();

	// emit signals to update GUI
	emit updateTopTenWidget(&topten);
	emit updateHistoryWidget(removed, item, incoming);

	// delete unused item
	if( removed && !removed->bookmarked() && !removed->m_in_topten /*&& !removed->m_in_history*/ ) {
		emit destroyIt(removed);
		dir.remove(removed);
	}

	return item;
}

AdbItem *AddressBook::insert( 
	const QString& id, const QString& nick, 
	const QString& givenName, const QString& surname, 
	const QString& email, const QString& telephone, 
	const QString& mobile, const QString& street, 
	const QString& city, const QString& code, 
	const QString& state, const QString& organization, 
	const QString& employeeNumber, const QString& employeeType, 
	const QString& departmentNumber, const QString& url, 
	const QString& picture )
{
	//create new item
	AdbItem *item = new AdbItem( id, nick, givenName, surname, email, telephone, 
			mobile, street, city, code, state, organization, employeeNumber, 
			employeeType, departmentNumber, url, picture );

	//insert new item to directory
	dir.append(item);

	return item;
}

void AddressBook::updateTopTen()
{
	AdbItemList items;
	AdbItemList newTopTen;

	//take only items that are outcoming in history
	for( AdbItem *item = dir.first(); item; item = dir.next() ) {
		if( item->m_outcoming )
			items.append(item);
	}

	items.sort(history.count());

	// take first 10 items from 'items' list
	int i = 0;
	for( AdbItem *item = items.first(); item && i < 10; item = items.next(), i++ ) {
		newTopTen.append(item);
	}
	
	// if there wasn't 10 different items in history, take some from the old history
	if( i < 10 ) {
		AdbItem *oldItem = topten.first();
		for( int j = i; j < 10; j++ ) {
			while( oldItem && newTopTen.containsRef(oldItem) ) oldItem = topten.next();
			if( !oldItem ) break;
			newTopTen.append(oldItem);
		}
	}

	for( AdbItem *item = topten.first(); item; item = topten.next() )
		item->m_in_topten = false;
	for( AdbItem *item = newTopTen.first(); item; item = newTopTen.next() )
		item->m_in_topten = true;

	topten = newTopTen;
}

void AddressBook::bookmarkItem( AdbItem *item, 
	const QString& id,
	const QString& nick, const QString& givenName, 
	const QString& surname, const QString& email, 
	const QString& telephone, const QString& mobile,
	const QString& street, const QString& city, 
	const QString& code, const QString& state,
	const QString& organization, const QString& employeeNumber, 
	const QString& employeeType, const QString& departmentNumber,
	const QString& url, const QString& picture )
{
	if( !item ) return;

	bool b = item->bookmarked();

	item->setId( id );
	item->setNick( nick );
	item->setGivenName( givenName );
	item->setSurname( surname );
	item->setEmail( email );
	item->setTelephone( telephone );
	item->setMobile( mobile );
	item->setStreet( street );
	item->setCity( city );
	item->setCode( code );
	item->setState( state );
	item->setOrganization( organization );
	item->setEmployeeNumber( employeeNumber );
	item->setEmployeeType( employeeType );
	item->setDepartmentNumber( departmentNumber );
	item->setURL( url );

	item->setPicture(picture);
	item->setBookmarked(true);

	if( !b )
		emit bookmarked(item);
	else
		emit edited(item);
}

void AddressBook::unbookmarkItem( AdbItem *item )
{
	if( !item ) return;

	item->setBookmarked(false);

	if( !item->m_in_topten && !item->m_in_history ) {
		emit destroyIt(item);
		dir.remove(item);
	}
	else
		emit unbookmarkIt(item);
}

void AddressBook::load()
{
	QStringList contacts = ancaConf->readListEntry( "addressbook", "contacts", QStringList() );
	QStringList hist = ancaConf->readListEntry( "addressbook", "history", QStringList() );

	for( QStringList::Iterator it = contacts.begin(); it != contacts.end(); ++it ) {
		QString line = *it;
		bool bookmarked = (line[0] == '0' ? false : true);
		if( bookmarked ) {
			dir.append(new AdbItem( 
				line.section('|', 1, 1)/*id*/,
				line.section('|', 2, 2)/*nick*/,
				line.section('|', 3, 3)/*givenName*/,
				line.section('|', 4, 4)/*surname*/,
				line.section('|', 5, 5)/*email*/,
				line.section('|', 6, 6)/*telephone*/,
				line.section('|', 7, 7)/*mobile*/,
				line.section('|', 8, 8)/*street*/,
				line.section('|', 9, 9)/*city*/,
				line.section('|', 10, 10)/*code*/,
				line.section('|', 11, 11)/*state*/,
				line.section('|', 12, 12)/*organization*/,
				line.section('|', 13, 13)/*employeeNumber*/,
				line.section('|', 14, 14)/*employeeType*/,
				line.section('|', 15, 15)/*departmentNumber*/,
				line.section('|', 16, 16)/*url*/,
				line.section('|', 17, 17)/*picture*/) );
		}
		else {
			dir.append(new AdbItem( line.section('|', 1, 1)/*url*/));
		}
	}

	for( QStringList::Iterator it = hist.begin(); it != hist.end(); ++it ) {
		bool incoming = (*it).left(1).toInt();
		int index = (*it).remove(0, 1).toInt();
		AdbItem *item = dir.at(index);
		if( !item ) continue;

		item->incInHistory(!incoming);
		history.insert(item, incoming);
	}

	updateTopTen();

	// emit signals to update GUI
	emit updateTopTenWidget(&topten);
	emit fillHistoryWidget(&history);
	emit fillAllWidget(&dir);
}

void AddressBook::save()
{
	QStringList contacts;
	for( AdbItemDirectoryIterator it(dir); it.current(); ++it ) {
		AdbItem *item = it.current();
		QString line;
		if( item->bookmarked() ) {
			line += "1|";
			line += item->id() + "|";
			line += item->nick() + "|";
			line += item->givenName() + "|";
			line += item->surname() + "|";
			line += item->email() + "|";
			line += item->telephone() + "|";
			line += item->mobile() + "|";
			line += item->street() + "|";
			line += item->city() + "|";
			line += item->code() + "|";
			line += item->state() + "|";
			line += item->organization() + "|";
			line += item->employeeNumber() + "|";
			line += item->employeeType() + "|";
			line += item->departmentNumber() + "|";
			line += item->url() + "|";
			line += item->picture();
		}
		else {
			line += "0|";
			line += item->url();
		}
		contacts.append(line);
	}

	//save history
	QStringList hist;
	for( QValueList<QPair<AdbItem*, bool> >::iterator it = history.begin(); it != history.end(); ++it ) {
		AdbItem *item = (*it).first;
		bool incoming = (*it).second;
		hist.append( QString(incoming ? "1" : "0") + QString::number(dir.find(item)) );
	}

	ancaConf->writeListEntry( "addressbook", "contacts", contacts );
	ancaConf->writeListEntry( "addressbook", "history", hist );
}

void AddressBook::deleteUnused()
{
	AdbItemDirectoryIterator it(dir);
	while( it.current() ) {
		AdbItem *item = it.current();

		if( !item->bookmarked() && !item->m_in_topten && !item->m_in_history ) {
			emit destroyIt(item);
			//if item is removed the iterator points to the next item
			if( dir.remove(item) ) continue;
		}

		++it;
	}
}

