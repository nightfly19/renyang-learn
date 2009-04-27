#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <qobject.h>
#include <qstring.h>
#include <qptrqueue.h>
#include <qptrlist.h>

/**
 * @short This class represents one item of addressbook
 *
 * Items can be of two types: bookmarked and not bookmarked. Bookmarked item means
 * that it is saved and can never be deleted until used does it explicitly. 
 * Unbookmarked item consist just from url and can be deleted automaticaly.
 */
class AdbItem
{
	friend class AddressBook;
	friend class AdbItemList;
public:
	/**
	 * Copy constructor
	 */
	AdbItem( const AdbItem& );

	/**
	 * Constructor for unbookmarked (and non LDAP) contact
	 */
 	AdbItem( const QString& url );

	/**
	 * @short Constructor for bookmarked contact. 
	 *
	 * It is used for loading addressbook from file.
	 *
	 * If this is an LDAP contact, \p id is an uid of the item in the
	 * directory (and \p url is meant to be alias).  Otherwise \p id it is
	 * null (and \p url is the whole h323 address).
	 */
 	AdbItem( const QString& id,
		const QString& nick, const QString& givenName, const QString& surname, 
		const QString& email, const QString& telephone, const QString& mobile,
		const QString& street, const QString& city, const QString& code, const QString& state,
		const QString& organization, const QString& employeeNumber, const QString& employeeType, 
		const QString& departmentNumber,
		const QString& url, const QString& picture, bool bookmarked = true );


	~AdbItem();

	/**
	 * Is this item in LDAP directory?
	 */
	bool inLDAP() const;

	/**
	 * Is this item bookmarked?
	 */
	bool bookmarked() const;

	/**
	 * Returns given name of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& givenName() const;

	/**
	 * Returns nickname of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& nick() const;

	/**
	 * Returns surname of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& surname() const;

	/**
	 * Returns email address of the contact. This is set only if the item
	 * is bookmarked.
	 */
	const QString& email() const;

	/**
	 * Returns telephone number of the contact. This is set only if the
	 * item is bookmarked.
	 */
	const QString& telephone() const;

	/**
	 * Returns mobile number of the contact. This is set only if the item
	 * is bookmarked.
	 */
	const QString& mobile() const;

	/**
	 * Returns street of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& street() const;

	/**
	 * Returns city of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& city() const;

	/**
	 * Returns postal code of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& code() const;

	/**
	 * Returns state of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& state() const;

	/**
	 * Returns ID of the contact. This is set only if the item is
	 * bookmarked as an LDAP contact. ID is unique identifier of the
	 * contact in LDAP directory.
	 */
	const QString& id() const;

	/**
	 * Returns organization of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& organization() const;

	/**
	 * Returns employeeNumber of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& employeeNumber() const;

	/**
	 * Returns employeeType of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& employeeType() const;

	/**
	 * Returns departmentNumber of the contact. This is set only if the item is
	 * bookmarked.
	 */
	const QString& departmentNumber() const;

	/**
	 * Returns file path of the picture of the contact. This is set only if
	 * the item is bookmarked.
	 */
	const QString& picture() const;


	/**
	 * Returns url of the contact.
	 *
	 * For LDAP contact it means an alias, otherwise it is whole h323 address.
	 */
	const QString& url() const;

	/**
	 * Sets some custom data.
	 * @see setCustomData
	 */
	void setCustomData( void *data );
	/**
	 * Returns saved custom data.
	 * @see setCustomData
	 */
	void *customData();

protected:
	void setNick( const QString& nick );
	void setGivenName( const QString& givenName );
	void setSurname( const QString& surname );
	void setEmail( const QString& email );
	void setTelephone( const QString& telephone );
	void setMobile( const QString& mobile );
	void setStreet( const QString& street );
	void setCity( const QString& city );
	void setCode( const QString& code );
	void setState( const QString& state );
	void setOrganization( const QString& organization );
	void setEmployeeNumber( const QString& employeeNumber );
	void setEmployeeType( const QString& employeeType );
	void setDepartmentNumber( const QString& departmentNumber );

	/**
	 * Changes also m_inLDAP flag.
	 */
	void setId( const QString& id );
	void setURL( const QString& url );

	void setPicture( const QString& picture );
	void setBookmarked( bool set = true );

	int incInHistory(bool outcoming);
	int decInHistory(bool outcoming);

	bool m_inLDAP;

	QString m_nick;
	QString m_givenName;
	QString m_surname;
	QString m_email;
	QString m_telephone;
	QString m_mobile;
	QString m_street;
	QString m_city;
	QString m_code;
	QString m_state;
	QString m_organization;
	QString m_employeeNumber;
	QString m_employeeType;
	QString m_departmentNumber;

	QString m_id;
	QString m_url;

	QString m_picture;

	/**
	 * How many times is this item in history
	 */
	int m_in_history;

	/**
	 * How many times this item represents outcoming contact in history
	 */
	int m_outcoming;

	/**
	 * Is the item in the toppen list?
	 */
	bool m_in_topten;

	bool m_bookmarked;

	bool m_was_called;

	void *data;
};

/**
 * @short This class represents the queue of AdbItems.
 *
 * It is stored as a QValueList of QPair<AdbItem*, bool incoming> items.
 */
class HistoryQueue: public QValueList<QPair<AdbItem*, bool> >
{
public:
	/**
	 * @short Constructor
	 * @param size How many items can be in the queue
	 */
	HistoryQueue( unsigned size );
	/**
	 * @short Copy constructor
	 */
	HistoryQueue( const HistoryQueue& );

	/**
	 * @short Destructor
	 */
	~HistoryQueue();

	/**
	 * @returns Item that was removed (in case that the queue was full)
	 */
	AdbItem *insert( AdbItem *item, bool incoming );

protected:
	unsigned size;
};
	
class AdbItemList: public QPtrList<AdbItem>
{
public:
	/**
	 * @brief Sorts the list.
	 *
	 * The radix sort algorithm is used. It overrides the Qt's heap sort
	 * because radix sort holds ordering
	 *
	 * @param maxSize Maximum number in the list
	 */
	void sort( int maxSize );
};
typedef QPtrListIterator<AdbItem> AdbItemListIterator;

class AdbItemDirectory: public AdbItemList
{
protected:
	/**
	 * @short This is virtual function that compares two AdbItems and is used when sorting.
	 *
	 * It compares url fields of AdbItems. It is useful when searching for
	 * AdbItem with the same url by QPtrList::contains()
	 *
	 * @returns see  QPtrList::compareItems()
	 */ 
	int compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
	{
		AdbItem *i1 = (AdbItem*)item1;
		AdbItem *i2 = (AdbItem*)item2;

		return QString::compare( i1->url(), i2->url() );
	}
};
typedef AdbItemListIterator AdbItemDirectoryIterator;


/**
 * @short This class represents the addressbook.
 *
 * There are three lists:
 * - main directory of all AdbItem s (bookmarked and unbookmarked)
 * - history list - history of requests (calls)
 * - topten list - consists of ten most popular contacts based on history
 */
class AddressBook: public QObject
{
	Q_OBJECT
public:
	/** Constructor */
	AddressBook();
	/** Destructor */
	~AddressBook();

	/**
	 * @short Inserts new (unbookmarked) AdbItem with url to directory or
	 * (if it is already there) update states of the existing AdbItem with
	 * url \p url.
	 *
	 * It updates history and topten lists, emits appropriate signals
	 * (updateTopTenWidget, updateHistoryWidget) and delete one item if it
	 * is not bookmarked and not in topten neither history list (then emits
	 * destroyIt signal)
	 *
	 * @param url URL to insert 
	 * @param incoming \p url can be incoming (the \p url is calling me) 
	 * or outcoming (I am calling the \p url)
	 * @returns AdbItem that was inserted/updated
	 */
	AdbItem *insert( const QString& url, bool incoming = false );

	/**
	 * @short Insert new (bookmarked) AdbItem to directory.
	 *
	 * Picture can be "default". It doesn't update history and topten lists.
	 * See AdbItem::AdbItem() for more details about parameters.
	 *
	 * @returns Item that was inserted to directory.
	 */
 	AdbItem *insert( 
		const QString& id, const QString& nick, 
		const QString& givenName, const QString& surname, 
		const QString& email, const QString& telephone, 
		const QString& mobile, const QString& street, 
		const QString& city, const QString& code, 
		const QString& state, const QString& organization, 
		const QString& employeeNumber, const QString& employeeType, 
		const QString& departmentNumber, const QString& url, 
		const QString& picture );

	/**
	 * @short Bookmark AdbItem.
	 *
	 * When the item was not bookmarked it is bookmarked and bookmarked()
	 * signal is emitted.  When the item was bookmarked the function works
	 * like some kind of update - it emits edited() signal.
	 */
 	void bookmarkItem( AdbItem *item, 
		const QString& id = QString::null,
		const QString& nick = QString::null, const QString& givenName = QString::null, 
		const QString& surname = QString::null, const QString& email = QString::null, 
		const QString& telephone = QString::null, const QString& mobile = QString::null,
		const QString& street = QString::null, const QString& city = QString::null, 
		const QString& code = QString::null, const QString& state = QString::null,
		const QString& organization = QString::null, const QString& employeeNumber = QString::null, 
		const QString& employeeType = QString::null, const QString& departmentNumber = QString::null,
		const QString& url = QString::null, const QString& picture = QString::null );

	/**
	 * @short Unbookmark AdbItem
	 *
	 * It is something like soft delete. AdbItem is not deleted until it is
	 * in none of the list (topten and history). Then the destroyIt()
	 * signal is emitted. When it is not deleted (just flag bookmarked is
	 * unset) signal unbookmarkIt() is emitted.
	 */
	void unbookmarkItem( AdbItem *item );

	/**
	 * Load contacts from AncaInfo.
	 */
	void load();

	/**
	 * Save contacts to AncaInfo.
	 */
	void save();

signals:
	/**
	 * Update of history widget is needed because toRemove item was removed
	 * and toInsert item was inserted to history. \p toInsert is either
	 * incoming or outcoming.
	 */
	void updateHistoryWidget( AdbItem *toRemove, AdbItem *toInsert, bool incoming );
	/**
	 * Update of topten widget is needed
	 * @param topten new topten list
	 */
	void updateTopTenWidget( AdbItemList *topten );

	/**
	 * Fill in the history widget with the items from history list.
	 * It is sent on startup when history was read from a file.
	 */
	void fillHistoryWidget( HistoryQueue *history );
	/**
	 * Fill in the all widget with the items from directory list.
	 * It is sent on startup when contacts were read from a file.
	 */
	void fillAllWidget( AdbItemDirectory *directory );

	/**
	 * Remove the AdbItem from all widgets. It is going to be deleted.
	 */
	void destroyIt( AdbItem * );
	/**
	 * Remove the AdbItem from main contact widget, because the item is
	 * no longed bookmarked.
	 */
	void unbookmarkIt( AdbItem * );

	/**
	 * The AdbItem was bookmarked so insert it to main contact widget
	 */
	void bookmarked( AdbItem * );
	/**
	 * The AdbItem was changed. Update it in all widgets it is in.
	 */
	void edited( AdbItem * );

protected:
	AdbItem *search( const QString& url );
	void updateTopTen();

	/**
	 * This function is no longer used.
	 */
	void deleteUnused();

	AdbItemDirectory dir;
	AdbItemList topten;
	HistoryQueue history;
};

#endif
