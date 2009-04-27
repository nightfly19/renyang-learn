
#ifndef LISTVIEWITEM_H
#define LISTVIEWITEM_H

#include <qlistview.h>

#include "addressbook.h"

class ListView : public QListView
{
	friend class ListViewItem;
public:

	/**
	 * Constructor.
	 *
	 * The parameters @p parent and @p name are handled by
	 * QListView, as usual.
	 */
	ListView (QWidget *parent = 0, const char *name = 0);

	/**
	 * Destructor.
	 */
	virtual ~ListView();

	/**
	 * sets the alternate background background color.
	 * This only has an effect if the items are ListViewItems
	 *
	 * @param c the color to use for every other item. Set to an invalid
	 *        color to disable alternate colors.
	 *
	 * @see alternateBackground()
	 **/
	void setAlternateBackground(const QColor &c);

	/**
	 * @return the alternate background color
	 *
	 * @see setAlternateBackground()
	 */
	const QColor &alternateBackground() const;

protected:
	/**
	 * Reimplemented for internal reasons.
	 * Further reimplementations should call this function or else
	 * some features may not work correctly.
	 *
	 * The API is unaffected.
	 */
	virtual void viewportPaintEvent(QPaintEvent*);

private:
	class ListViewPrivate;
	ListViewPrivate *d;
};

class ListViewItem : public QListViewItem
{
public:
	ListViewItem( QListView *parent, AdbItem *adbItem, int textColumn );
	ListViewItem( QListView *parent, AdbItem *adbItem, int textColumn, QListViewItem *after );
	ListViewItem( QListView *parent, const QString& text, const QPixmap& pixmap );

	~ListViewItem();

	/**
	 * returns true if this item is to be drawn with the alternate background
	 */
	bool isAlternate();

	/**
	 * returns the background color for this item
	 */
	const QColor &backgroundColor();


	AdbItem *getAdbItem();

	void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );

	int compare( QListViewItem *i, int col, bool ) const;

	virtual void reset();

protected:
	AdbItem *adbItem;

private:
	uint m_odd : 1;
	uint m_known : 1;
	uint m_unused : 30;

	int textColumn;
	QString sortingText;
};

class HistoryItem: public ListViewItem
{
public:
	HistoryItem( QListView *parent, AdbItem *adbItem, int textColumn, int typeColumn, bool incoming );
	HistoryItem( QListView *parent, AdbItem *adbItem, int textColumn, int typeColumn, bool incoming, QListViewItem *after );
	
	virtual void reset();

protected:
	int typeColumn;
	bool incoming;
};

#endif

