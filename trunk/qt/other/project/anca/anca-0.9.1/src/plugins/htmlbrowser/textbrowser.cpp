#include "textbrowser.h"
#include "netmime.h"

#include <qurloperator.h>

TextBrowser::TextBrowser( QWidget *parent, const char *name )
	:QTextBrowser(parent, name), m_linksEnabled(true)
{
	netMime = new NetMimeSourceFactory();
	setMimeSourceFactory( netMime );
}

TextBrowser::~TextBrowser()
{
}

void TextBrowser::setSource(const QString& name_)
{
	QString name = name_;
	if( !name.startsWith("http://") )
		name.prepend("http://");
	QUrl url(name);

	if( url.hasHost() ) {
		netMime->setHost(url.host());
		netMime->setDir(url.dirPath());
	}
	else {
		netMime->setHost();
		netMime->setDir();
	}

	m_linksEnabled = false;
	QTextBrowser::setSource(name);
	m_linksEnabled = true;
}

bool TextBrowser::linksEnabled() const
{ 
	return m_linksEnabled;
}
