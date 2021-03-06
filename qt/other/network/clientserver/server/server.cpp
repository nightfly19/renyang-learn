/****************************************************************************
** $Id: qt/server.cpp   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qsocket.h>
#include <qserversocket.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qtextview.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextstream.h>

#include <stdlib.h>


/*
  The ClientSocket class provides a socket that is connected with a client.
  For every client that connects to the server, the server creates a new
  instance of this class.
*/
// 建立一個base在tcp上的client端類別
class ClientSocket : public QSocket
{
    Q_OBJECT
public:
    ClientSocket( int sock, QObject *parent=0, const char *name=0 ) : QSocket( parent, name )
    {
	qWarning(QString("ClientSocket::ClientSocket(%1,QObject *parent,const char *name)").arg(sock));
	line = 0;
	// readyRead()每當有資料可以被讀取時, readyRead()會被系統自動emit[這是系統內定的]
	// 也就是說由client有資料要被送過來時, readyRead()會被自動呼叫
	connect( this, SIGNAL(readyRead()), SLOT(readClient()) );
	// 當connectionClosed()被執行時, 延期刪除此物件(this)
	connect( this, SIGNAL(connectionClosed()), SLOT(deleteLater()) );
	// 設定目前這一個ClientSocket使用的socket file descriptor
	// 必需注意的是此sock必需要是connected
	setSocket( sock );
    }

    ~ClientSocket()
    {
    }

signals:
    void logText( const QString& );

private slots:
    void readClient()
    {
    	qWarning(QString("ClientSocket::readClient()"));

	// QTextStream(QIODevice * iod)設定的是一個IO Device
	// 這一個應該是重點, 把QTextStream與socket所建立的file descriptor所連接起來
	// 同時, 這一個socket file descriptor代表的就是client端的QIODevice
	QTextStream ts( this );
	// 是否可以由目前的socket讀出完整的字串, 我猜是因為'\n'字元來判定的readLine
	while ( canReadLine() ) {
	    QString str = ts.readLine();
	    // 送到要顯示在server端的介面
	    emit logText( tr("Read: '%1'\n").arg(str) );

	    // 回送給client端資料
	    ts << line << ": " << str << endl;
	    // 顯示回送給client端的資料到server端的介面
	    emit logText( tr("Wrote: '%1: %2'\n").arg(line).arg(str) );

	    line++;
	}
    }

private:
    // 記錄送出的資料行數
    int line;
};


/*
  The SimpleServer class handles new connections to the server. For every
  client that connects, it creates a new ClientSocket -- that instance is now
  responsible for the communication with that client.
*/
// QServerSocket是提供一個TCP base的server類別
class SimpleServer : public QServerSocket
{
    Q_OBJECT
public:
    // 使用port為4242, 同時只能讓一個client連到server
    SimpleServer( QObject* parent=0 ) : QServerSocket( 4242, 1, parent )
    {
	qWarning(QString("SimpleServer::SimpleServer(QObject *parent)"));
	// ok()回傳建構子是否成功, 也就是是否傳統中的bind(), listen(), accept()這三個動作是否成功
	// 在Qt中, 它把它連成bind(), listen(), accept()三個部份設定在一起
	if ( !ok() ) {
	    qWarning("Failed to bind to port 4242");
	    exit(1);
	}
    }

    ~SimpleServer()
    {
	qWarning(QString("SimpleServer::~SimpleServer()"));
    }

    // 當建立一個新連線時, 則產生一個client端的物件
    // 噹噹, 接收新的連線, 這一個函數是一個重點, 當server接收到一個新的連線要求時, 會呼叫此函式
    // 而socket就是代表client端的socket file descriptor
    // 而要利用此socket file descriptor去建立ClientSocket物件
    void newConnection( int socket )
    {
    	qWarning(QString("SimpleServer::newConnection(%1)").arg(socket));
	// 在server端由取得的socket file descriptor來產生一個client的物件
	ClientSocket *s = new ClientSocket( socket, this );
	// 通知建立一個新的連線啦
	emit newConnect( s );
    }

signals:
    void newConnect( ClientSocket* );	// 產生一個新連線通知
};


/*
  The ServerInfo class provides a small GUI for the server. It also creates the
  SimpleServer and as a result the server.
*/
class ServerInfo : public QVBox
{
    Q_OBJECT
public:
    ServerInfo()
    {
	qWarning(QString("ServerInfo::ServerInfo()"));
	// 建立一個tcp base server
	SimpleServer *server = new SimpleServer( this );

	// 要顯示在server介面上的訊息欄位
	QString itext = tr(
		"This is a small server example.\n"
		"Connect with the client now."
		);
	QLabel *lb = new QLabel( itext, this );
	lb->setAlignment( AlignHCenter );
	infoText = new QTextView( this );
	QPushButton *quit = new QPushButton( tr("Quit") , this );

	// 當server有新的連線進來時, 通知newConnect函式會被emit[這是系統內定的]
	connect( server, SIGNAL(newConnect(ClientSocket*)),this,SLOT(newConnect(ClientSocket*)) );
	// 離開應用程式
	connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
    }

    ~ServerInfo()
    {
	qWarning(QString("ServerInfo::~ServerInfo()"));
    }

private slots:
    void newConnect( ClientSocket *s )
    {
	qWarning(QString("ServerInfo::newConnect()"));
	// 顯示有新的連線
	infoText->append( tr("New connection\n") );
	// 當由ClientSocket收到資料, 則會顯示在infoText中
	// 當有人由client輸入資料，則會顯示在server端中
	connect( s, SIGNAL(logText(const QString&)), infoText, SLOT(append(const QString&)) );
	connect( s, SIGNAL(connectionClosed()), this, SLOT(connectionClosed()) );
    }

    void connectionClosed()
    {
	qWarning(QString("ServerInfo::connectionClosed()"));
	infoText->append( tr("Client closed connection\n") );
    }

private:
    // 在server端要顯示連線與斷線的訊息, 均透過infoText
    QTextView *infoText;
};


int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    ServerInfo info;
    app.setMainWidget( &info );
    info.show();
    return app.exec();
}

#include "server.moc"
