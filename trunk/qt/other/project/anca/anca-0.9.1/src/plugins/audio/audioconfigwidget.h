#ifndef AUDIOCONFIGWIDGET_H
#define AUDIOCONFIGWIDGET_H

#include "plugin.h"
#include "audioconfigwidget.h"

#include <qlistview.h>

class QVBoxLayout;
class CodecViewItem;
class AudioCodecsWidget;
class AudioConfigWidget;

class CapabilityItem
{
public:
	CapabilityItem( const QString& name, const QString& caption, const QString& bandwidth );
	~CapabilityItem();

	QString getName() { return name; }
	void setName( const QString& name_ ) { name = name_; };
	QString getCaption() { return caption; }
	QString getBandwidth() { return bandwidth; }

	bool isUsed() { return used; }
	void setUsed( bool used_ = true ) { used = used_; }

private:
	QString name;
	QString caption;
	QString bandwidth;
	bool used;
};

class AudioConfigWidget: public ConfigWidget
{
	Q_OBJECT
	friend class CodecViewItem;
public:
	AudioConfigWidget( H323EndPoint *endPoint, QWidget *parent = 0, const char *name = 0 );
	~AudioConfigWidget();

	void emitChanged();

public slots:
	void apply();
	void reset();

signals:
	void apply( const QValueList<CapabilityItem*>& capabilities );
	void resetView();

protected:
	H323EndPoint *endPoint;
	AudioCodecsWidget *widget;
	QVBoxLayout* AudioConfigWidgetLayout;
	QValueList<CapabilityItem*> capabilities;

protected slots:
	void moveDown();
	void moveUp();
};

class CodecViewItem: public QCheckListItem
{
public:
	CodecViewItem( AudioConfigWidget *config, CapabilityItem *cap );
	CodecViewItem( AudioConfigWidget *config, CodecViewItem *after, CapabilityItem *cap );
	~CodecViewItem();

	CapabilityItem *getCapability() { return cap; }

protected:
	void stateChange( bool on );
	CapabilityItem *cap;
	AudioConfigWidget *config;

private:
	bool inConstructor;
};
	

#endif
