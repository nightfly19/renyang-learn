#ifndef NET_MIME_H
#define NET_MIME_H

#include <qmime.h>
#include <qobject.h>

class QUrlOperator;
class QNetworkOperation;
class NetMimeSourceFactoryData;

class NetMimeSourceFactory: public QObject, public QMimeSourceFactory
{
	Q_OBJECT;
public:
	NetMimeSourceFactory();
	~NetMimeSourceFactory();

	virtual const QMimeSource* data(const QString& abs_name) const;
	virtual void setExtensionType( const QString& ext, const char* mimetype );

	void setHost( const QString& host = QString::null ) { curHost = host; }
	void setDir( const QString& dir = QString::null ) { curDir = dir; }

public slots:
	void downloaded( QNetworkOperation *op );
	void newData( const QByteArray& data, QNetworkOperation *op );

private:
	QMimeSource* dataInternal(const QString& abs_name) const;

	/**
	 * Current path.
	 */
	QString curDir;
	/**
	 * Current host.
	 */
	QString curHost;

	NetMimeSourceFactoryData *d;
};

#endif
