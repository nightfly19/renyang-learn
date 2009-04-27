#ifndef TEXTBROWSER_H
#define TEXTBROWSER_H

#include <qtextbrowser.h>
#include <qmime.h>

class NetMimeSourceFactory;

class TextBrowser: public QTextBrowser
{
	Q_OBJECT
public:
	TextBrowser( QWidget* parent=0, const char* name=0 );
	~TextBrowser();

public slots:
	virtual void setSource(const QString& name);

protected:
	NetMimeSourceFactory *netMime;

private:
	virtual bool linksEnabled() const;
	bool m_linksEnabled;

};

#endif
