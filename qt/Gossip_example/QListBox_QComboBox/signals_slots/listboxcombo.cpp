#include "listboxcombo.h"

#include <qcombobox.h>
#include <qlistbox.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpainter.h>
#include <qstyle.h>

/*
* Constructor
*
* Creates child widgets of the ListBoxCombo widget
*/

ListBoxCombo::ListBoxCombo( QWidget *parent, const char *name )
    : QVBox( parent, name )
{
    setMargin( 5 );
    setSpacing( 5 );

    unsigned int i;
    QString str;

    QHBox *row1 = new QHBox( this );
    row1->setSpacing( 5 );

    // Create a multi-selection ListBox...
    lb1 = new QListBox( row1 );
    lb1->setSelectionMode( QListBox::Multi );

    // ...insert a pixmap item...
    lb1->insertItem( QPixmap( "dog.xpm" ) );
    // ...and 100 text items
    for ( i = 0; i < 100; i++ ) {
        str = QString( "Listbox Item %1" ).arg( i );
        if ( !( i % 4 ) )
            lb1->insertItem( QPixmap( "fileopen.xpm" ), str );
        else
            lb1->insertItem( str );
    }

    // Create a pushbutton...
    QPushButton *arrow1 = new QPushButton( " -> ", row1 );
    // ...and connect the clicked SIGNAL with the SLOT slotLeft2Right
    connect( arrow1, SIGNAL( clicked() ), this, SLOT( slotLeft2Right() ) );

    // create an empty single-selection ListBox
    lb2 = new QListBox( row1 );

    QHBox *row2 = new QHBox( this );
    row2->setSpacing( 5 );

    QVBox *box1 = new QVBox( row2 );
    box1->setSpacing( 5 );

    // Create an editable Combobox and a label below...
    QComboBox *cb1 = new QComboBox( false, box1 );
    label2 = new QLabel( "Current Item: Combobox Item 0", box1 );
    label2->setMaximumHeight( label2->sizeHint().height() * 2 );
    label2->setFrameStyle( QFrame::Panel | QFrame::Sunken );

    // ... and insert 50 items into the Combobox
    for ( i = 0; i < 50; i++ ) {
        str = QString( "Combobox Item %1" ).arg( i );
        if ( !( i % 4 ) )
            cb1->insertItem( QPixmap( "fileopen.xpm" ), str );
        else
            cb1->insertItem( str );
    }

    // Connect the activated SIGNALs of the Comboboxes with SLOTs
    connect( cb1, SIGNAL( activated( const QString & ) ), 
             this, SLOT( slotCombo1Activated( const QString & ) ) );
}

/*
* SLOT slotLeft2Right
*
* Copies all selected items of the first ListBox into the
* second ListBox
*/

void ListBoxCombo::slotLeft2Right()
{
    // Go through all items of the first ListBox
    for ( unsigned int i = 0; i < lb1->count(); i++ ) {
         QListBoxItem *item = lb1->item( i );
   // if the item is selected...
        if ( item->isSelected() ) {
             // ...and it is a text item...
             if ( item->pixmap() && !item->text().isEmpty() )
                lb2->insertItem( *item->pixmap(), item->text() );
             else if ( !item->pixmap() )
                lb2->insertItem( item->text() );
             else if ( item->text().isEmpty() )
                lb2->insertItem( *item->pixmap() );
        }
    }
}

/*
* SLOT slotCombo1Activated( const QString &s )
*
* Sets the text of the item which the user just selected
* in the second Combobox (and is now the value of s) to
* the second Label.
*/

void ListBoxCombo::slotCombo1Activated( const QString &s )
{
    label2->setText( QString( "Current Item: %1" ).arg( s ) );
}
