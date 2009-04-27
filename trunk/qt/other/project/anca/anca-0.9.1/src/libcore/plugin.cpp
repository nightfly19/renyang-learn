#include "plugin.h"

H323EndPoint *Plugin::endPoint = 0;
QString Plugin::currentCallToken;

Plugin::Plugin( PluginInfo *info ): info(info)
{
}

Plugin::~Plugin()
{
}

const char *Plugin::name() const
{
	return info ? info->name: 0;
}

Plugin::Type Plugin::type() const
{
	return info ? info->type : Other;
}

const char *Plugin::description() const
{
	return info ? info->description: 0;
}

const char *Plugin::author() const
{
	return info ? info->author: 0;
}

MediaPlugin::MediaPlugin( PluginInfo *info ): Plugin(info)
{
}

VideoInPlugin::VideoInPlugin( PluginInfo *info ): MediaPlugin(info)
{
}

VideoOutPlugin::VideoOutPlugin(PluginInfo *info ): Plugin(info)
{
}

AudioPlugin::AudioPlugin( PluginInfo *info ): MediaPlugin(info), bufferSize(0)
{
}

ConfigWidget::ConfigWidget( QWidget *parent, const char *name ): QWidget(parent, name)
{
}

ConfigWidget::~ConfigWidget()
{
	disconnect(this);
	disconnect(SIGNAL(changed()));
}
