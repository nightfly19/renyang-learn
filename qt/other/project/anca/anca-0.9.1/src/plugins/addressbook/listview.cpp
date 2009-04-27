#include "listview.h"

#include <qfontmetrics.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qpixmap.h>

class ListView::ListViewPrivate
{
public:
	ListViewPrivate (ListView* listview)
		: paintAbove (0),
		paintCurrent (0),
		paintBelow (0),
		painting (false)
	{}
	
	~ListViewPrivate ()
	{}
	
	QListViewItem *paintAbove;
	QListViewItem *paintCurrent;
	QListViewItem *paintBelow;
	bool painting;
	
	QColor alternateBackground;
};

ListView::ListView( QWidget *parent, const char *name )
	: QListView( parent, name ), d (new ListViewPrivate (this))
{
	d->alternateBackground = Qt::darkGray;
}

ListView::~ListView()
{
	delete d;
}

const QColor &ListView::alternateBackground() const
{
	return d->alternateBackground;
}

void ListView::setAlternateBackground(const QColor &c)
{
	d->alternateBackground = c;
	repaint();
}

void ListView::viewportPaintEvent(QPaintEvent *e)
{
	d->paintAbove = 0;
	d->paintCurrent = 0;
	d->paintBelow = 0;
	d->painting = true;

	QListView::viewportPaintEvent(e);

	d->painting = false;
}

///////////////////// ListViewItem ////////////////////////

ListViewItem::ListViewItem( QListView *parent, AdbItem *item, int textColumn )
	: QListViewItem(parent), adbItem(item), m_odd(false), m_known(false), textColumn(textColumn)
{
	reset();
}

/*!
    Constructs a list box item showing the text \a text.
*/

ListViewItem::ListViewItem( QListView *parent, AdbItem *item, int textColumn, QListViewItem *after )
	: QListViewItem( parent, after ), adbItem(item), m_odd(false), m_known(false), textColumn(textColumn)
{
	reset();
}

ListViewItem::ListViewItem( QListView *parent, const QString& text, const QPixmap& pixmap )
	: QListViewItem( parent ), adbItem(0), m_odd(false), m_known(false), sortingText(text)
{
	setPixmap(0,pixmap);
}

/*!
	Destroys the item.
*/

ListViewItem::~ListViewItem()
{
}

AdbItem *ListViewItem::getAdbItem()
{
	return adbItem;
}

const QColor &ListViewItem::backgroundColor()
{
	if (isAlternate())
		return static_cast< ListView* >(listView())->alternateBackground();
	return listView()->viewport()->colorGroup().base();
}

bool ListViewItem::isAlternate()
{
	ListView *lv = static_cast<ListView *>(listView());
	if (lv && lv->alternateBackground().isValid()) {
		ListViewItem *above;
		
		// Ok, there's some weirdness here that requires explanation as this is a
		// speed hack.  itemAbove() is a O(n) operation (though this isn't
		// immediately clear) so we want to call it as infrequently as possible --
		// especially in the case of painting a cell.
		//
		// So, in the case that we *are* painting a cell:  (1) we're assuming that
		// said painting is happening top to bottem -- this assumption is present
		// elsewhere in the implementation of this class, (2) itemBelow() is fast --
		// roughly constant time.
		//
		// Given these assumptions we can do a mixture of caching and telling the
		// next item that the when that item is the current item that the now
		// current item will be the item above it.
		//
		// Ideally this will make checking to see if the item above the current item
		// is the alternate color a constant time operation rather than 0(n).
		
		if (lv->d->painting) {
			if (lv->d->paintCurrent != this) {
				lv->d->paintAbove = lv->d->paintBelow == this ? lv->d->paintCurrent : itemAbove();
				lv->d->paintCurrent = this;
				lv->d->paintBelow = itemBelow();
			}
			
			above = dynamic_cast<ListViewItem *>(lv->d->paintAbove);
		}
		else {
			above = dynamic_cast<ListViewItem *>(itemAbove());
		}
		
		m_known = above ? above->m_known : true;
		if (m_known) {
			m_odd = above ? !above->m_odd : false;
		}
		else {
			ListViewItem *item;
			bool previous = true;
			if (parent()) {
				item = dynamic_cast<ListViewItem *>(parent());
				if (item)
					previous = item->m_odd;
				item = dynamic_cast<ListViewItem *>(parent()->firstChild());
			}
			else {
				item = dynamic_cast<ListViewItem *>(lv->firstChild());
			}
			
			while(item) {
				item->m_odd = previous = !previous;
				item->m_known = true;
				item = dynamic_cast<ListViewItem *>(item->nextSibling());
			}
		}
		return m_odd;
	}
	return false;
}

void ListViewItem::paintCell( QPainter * p, const QColorGroup & cg, int column, int width, int alignment )
{
	QColorGroup _cg = cg;
	const QPixmap *pm = listView()->viewport()->backgroundPixmap();
	if (pm && !pm->isNull()) {
		_cg.setBrush(QColorGroup::Base, QBrush(backgroundColor(), *pm));
		QPoint o = p->brushOrigin();
		p->setBrushOrigin( o.x()-listView()->contentsX(), o.y()-listView()->contentsY() );
	}
	else if (isAlternate())
		if (listView()->viewport()->backgroundMode()==Qt::FixedColor)
		     _cg.setColor(QColorGroup::Background, static_cast< ListView* >(listView())->alternateBackground());
		else
		 _cg.setColor(QColorGroup::Base, static_cast< ListView* >(listView())->alternateBackground());
	
	QListViewItem::paintCell(p, _cg, column, width, alignment);
}

int ListViewItem::compare( QListViewItem *i, int col, bool ascending ) const
{
	QString key1 = adbItem ? key( col, ascending ) : sortingText;
	QString key2 = ((ListViewItem*)i)->adbItem ? i->key( col, ascending ) : ((ListViewItem*)i)->sortingText;
	return key1.localeAwareCompare( key2 );
}

void ListViewItem::reset()
{
	if( !adbItem ) return;

	if( adbItem->bookmarked() || adbItem->inLDAP() )
		setText( textColumn, adbItem->nick() + " <" + adbItem->url() + ">" );
	else
		setText( textColumn, adbItem->url() );

	if( !adbItem->picture().isNull() )
		if( adbItem->picture() == "default" )
			setPixmap( textColumn, QPixmap::fromMimeSource("defaultPicture.png") );
		else
			setPixmap( textColumn, QPixmap(adbItem->picture()) );
	else
		setPixmap( textColumn, 0 );
}

///////////////////// HistoryItem ////////////////////////

HistoryItem::HistoryItem( QListView *parent, AdbItem *item, int textColumn, int typeColumn, bool incoming )
	:ListViewItem(parent, item, textColumn), typeColumn(typeColumn), incoming(incoming)
{
	reset();
}


HistoryItem::HistoryItem( QListView *parent, AdbItem *item, int textColumn, int typeColumn, bool incoming, QListViewItem *after )
	: ListViewItem( parent, item, textColumn, after ), typeColumn(typeColumn), incoming(incoming)
{
	reset();
}

void HistoryItem::reset()
{
	ListViewItem::reset();
	if( incoming )
		setPixmap( typeColumn, QPixmap::fromMimeSource( "incoming.png" ) );
	else
		setPixmap( typeColumn, QPixmap::fromMimeSource( "outcoming.png" ) );
}
