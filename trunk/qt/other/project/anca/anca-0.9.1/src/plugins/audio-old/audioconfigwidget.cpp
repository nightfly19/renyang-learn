#include "audioconfigwidget.h"
#include "audiocodecswidget.h"
#include <qlayout.h>
#include <qpushbutton.h>

#include <ptlib.h>
#include <h323.h>

AudioConfigWidget::AudioConfigWidget( H323EndPoint *endPoint, QWidget *parent, const char *name )
	: ConfigWidget(parent,name), endPoint(endPoint)
{
	AudioConfigWidgetLayout = new QVBoxLayout( this, 0, 6, "AudioConfigWidgetLayout");

	widget = new AudioCodecsWidget(this, "Audio codecs widget");
	AudioConfigWidgetLayout->addWidget(widget);

	widget->audioCodecsView->setSorting(-1);
	connect( widget->audioCodecsUpButton, SIGNAL( clicked() ), this, SLOT( moveUp() ) );
	connect( widget->audioCodecsDownButton, SIGNAL( clicked() ), this, SLOT( moveDown() ) );
}

AudioConfigWidget::~AudioConfigWidget()
{
}

void AudioConfigWidget::emitChanged()
{
	emit changed();
}

void AudioConfigWidget::apply()
{
	capabilities.clear();
	for( QListViewItem *i = widget->audioCodecsView->firstChild(); i; i = i->nextSibling() ) {
		CodecViewItem *item = (CodecViewItem*)i;
		capabilities.append(item->getCapability());
	}
	emit apply(capabilities);
}

void AudioConfigWidget::reset()
{
	widget->audioCodecsView->clear();
	emit resetView();
}

void AudioConfigWidget::moveDown()
{
	QListViewItem *item = widget->audioCodecsView->currentItem();
	if( !item || item == widget->audioCodecsView->lastItem() ) return;

	item->moveItem( item->itemBelow() );

	emit changed();
}

void AudioConfigWidget::moveUp()
{
	QListViewItem *item = widget->audioCodecsView->currentItem();
	if( !item || item == widget->audioCodecsView->firstChild() ) return;

	QListViewItem *afterItem = item->itemAbove()->itemAbove();
	if( afterItem )
		item->moveItem( afterItem );
	else 
		// We are on the second position and we want to move on the first.
		// So move the first one after ours one.
		item->itemAbove()->moveItem( item );

	emit changed();
}

/********************************************************************/

CodecViewItem::CodecViewItem( AudioConfigWidget *config, CapabilityItem *capability )
	:QCheckListItem(config->widget->audioCodecsView, QString::null, QCheckListItem::CheckBox), cap(capability), config(config), inConstructor(true)
{
	setText(1, cap->getBandwidth() );
	setText(2, cap->getCaption() );
	if( cap->isUsed() ) 
		setOn(true);
	else
                setOn(false);
	inConstructor = false;
}

CodecViewItem::CodecViewItem( AudioConfigWidget *config, CodecViewItem *after, CapabilityItem *capability )
	:QCheckListItem(config->widget->audioCodecsView, after, QString::null, QCheckListItem::CheckBox), cap(capability), config(config), inConstructor(true)
{
	setText(1, cap->getBandwidth() );
	setText(2, cap->getCaption() );
	if( cap->isUsed() ) 
                setOn(true);
	else
                setOn(false);
	inConstructor = false;
}

CodecViewItem::~CodecViewItem()
{
}

void CodecViewItem::stateChange( bool on )
{
	if( inConstructor ) return;

	cap->setUsed(on);
	config->emitChanged();
}

/********************************************************************/

CapabilityItem::CapabilityItem( const QString& name, const QString& caption, const QString& bandwidth )
	:name(name), caption(caption), bandwidth(bandwidth), used(false)
{
}

CapabilityItem::~CapabilityItem() 
{
}
