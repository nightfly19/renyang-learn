#include "netmime.h"

#include <qurloperator.h>
#include <qnetwork.h>
#include <qwaitcondition.h>
#include <qnetworkprotocol.h>
#include <qfileinfo.h>
#include <qapplication.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qmap.h>

#define INITIAL_BUF_SIZE 1000

class NetMimeSourceFactoryData {
public:
	NetMimeSourceFactoryData() : last(0), buf(INITIAL_BUF_SIZE), bufUsed(0)
	{
		urlOperator = new QUrlOperator();
	}

	~NetMimeSourceFactoryData()
	{
		delete last;
		delete urlOperator;
	}

	QMimeSource *last;
	QByteArray buf;
	int bufUsed;
	QWaitCondition cond;
	QUrlOperator *urlOperator;
	bool downloaded;
	QMap<QString, QString> extensions;

	QString lastHost;
	QString lastHostPath;
};

NetMimeSourceFactory::NetMimeSourceFactory(): d(new NetMimeSourceFactoryData)
{
	setExtensionType("html","text/html;charset=UTF-8");
	setExtensionType("htm","text/html;charset=UTF-8");
	setExtensionType("png", "image/png" );
	setExtensionType("jpg", "image/jpeg" );
	setExtensionType("jpeg", "image/jpeg" );
	setExtensionType("gif", "image/gif" );
	setExtensionType("PNG", "image/png" );
	setExtensionType("JPG", "image/jpeg" );
	setExtensionType("JPEG", "image/jpeg" );
	setExtensionType("GIF", "image/gif" );

	qInitNetworkProtocols();

	connect( d->urlOperator, SIGNAL(finished( QNetworkOperation * )), this, SLOT(downloaded( QNetworkOperation * )));
	connect( d->urlOperator, SIGNAL(data( const QByteArray &, QNetworkOperation * )), this, SLOT(newData( const QByteArray &, QNetworkOperation * )));
}

NetMimeSourceFactory::~NetMimeSourceFactory()
{
	delete d;
}

QMimeSource* NetMimeSourceFactory::dataInternal(const QString& name) const
{
//	printf("dataInternal( name = %s )\n", name.latin1());

	d->bufUsed = 0;
	d->downloaded = false;
	const QNetworkOperation *op = d->urlOperator->get(name);
	QString fileName = op->arg(0);
	QFileInfo fi(fileName);

	//cycle until anybody will wait the condition cond manually,
	//wait inactively 100 miliseconds between each iteration
	while( !d->cond.wait(100) ) {
		if( d->downloaded ) break;
		qApp->processEvents();
	}

	QCString mimetype = "text/html";

	if( !d->bufUsed ) {
		// we will print out html warning...
		mimetype = "text/html";
		QString message = "<h1 align=\"center\">Page not available</h1>";

		// copy message to buffer
		strcpy( d->buf.data(), message.latin1() );
		d->bufUsed = message.length();
	}
	else {
		// get the right mimetype
		QString e = fi.extension(FALSE);
		const char* imgfmt;
		if ( d->extensions.contains(e) )
			mimetype = d->extensions[e].latin1();
		else if ( ( imgfmt = QImage::imageFormat( fileName ) ) )
			mimetype = QCString("image/")+QCString(imgfmt).lower();
	}

	QByteArray ba;
	ba.duplicate( d->buf, d->bufUsed );
	QStoredDrag* sr = new QStoredDrag( mimetype );
	sr->setEncodedData( ba );
	delete d->last;
	d->last = sr;

//	printf( "format: %s\n", sr->format(0) );

	return sr;
}

const QMimeSource* NetMimeSourceFactory::data(const QString& abs_name) const
{
	QString name = abs_name;

	if( QUrl(name).hasHost() ) {
		if( !name.startsWith("http://") )
			name.prepend("http://");
	}
	else {
		if( name[0] == '/' )
			name = "http://" + curHost + name;
		else
			name = "http://" + curHost + "/" + curDir + name;

	}

	return dataInternal(name);
}

void NetMimeSourceFactory::setExtensionType( const QString& ext, const char* mimetype )
{
	d->extensions.replace(ext, mimetype);
}

void NetMimeSourceFactory::downloaded( QNetworkOperation *op )
{
//	printf("file %s downloaded with state %d\n", op->arg(0).latin1(), op->state() );
	if( op->state() != QNetworkProtocol::StDone )
		d->bufUsed = 0;

	d->cond.wakeAll();
	d->downloaded = true;
//	printf("main thread waken up\n");
}

void NetMimeSourceFactory::newData( const QByteArray& data, QNetworkOperation * /*op*/ )
{
//	printf("new data (%d) arrived!\n", data.size());

	uint requestedSize = d->bufUsed + data.size();
	if( requestedSize > d->buf.size() ) {
//		printf("resizing buffer to %d\n", requestedSize);
		if( !d->buf.resize( requestedSize ) )
			printf("!!!!!!!! very bad :-)\n");
	}

	// append data to buffer
	for( uint j = 0, i = d->bufUsed; i < requestedSize && i < d->buf.size() && j < data.size()/*not need it*/; i++, j++ )
		d->buf[i] = data[j];
	d->bufUsed = requestedSize;
//	printf("new data appended\n");
}

