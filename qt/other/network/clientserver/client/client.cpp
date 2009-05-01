/****************************************************************************
** $Id: qt/client.cpp   3.3.7   edited Aug 31 2005 $
**
** Copyright (C) 1992-2005 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qsocket.h>
#include <qapplication.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qtextview.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextstream.h>


class Client : public QVBox
{
    Q_OBJECT
public:
    Client( const QString &host, Q_UINT16 port )
    {
	qWarning(QString("Client::Client(%1,%2)").arg(host).arg(port));
	// GUI layout
	infoText = new QTextView( this );
	QHBox *hb = new QHBox( this );
	inputText = new QLineEdit( hb );
	QPushButton *send = new QPushButton( tr("Send") , hb );
	QPushButton *close = new QPushButton( tr("Close connection") , this );
	QPushButton *quit = new QPushButton( tr("Quit") , this );

	connect( send, SIGNAL(clicked()), SLOT(sendToServer()) );
	connect( close, SIGNAL(clicked()), SLOT(closeConnection()) );
	connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

	// create the socket and connect various of its signals
	// 建立一個tcp socket, 其父類別是this, 表示本地端的socket[重要!!]
	socket = new QSocket( this );
	// 當正確連線到server端, 系統會自動emit connected()[系統預設]
	connect( socket, SIGNAL(connected()), SLOT(socketConnected()) );
	// 當socket被切斷連線, 系統會自動emit connectionClosed()[系統預設]
	connect( socket, SIGNAL(connectionClosed()), SLOT(socketConnectionClosed()) );
	// 當socket接收到資料, readyRead()會同時被觸發[系統預設]
	connect( socket, SIGNAL(readyRead()), SLOT(socketReadyRead()) );
	// 當socket發現錯誤, 系統會自動被emit error(int)
	connect( socket, SIGNAL(error(int)), SLOT(socketError(int)) );

	// connect to the server
	infoText->append( tr("Trying to connect to the server\n") );
	socket->connectToHost( host, port );
    }

    ~Client()
    {
    }

private slots:
    void closeConnection()
    {
	qWarning(QString("Client::closeConnection()"));
	socket->close();
	if ( socket->state() == QSocket::Closing ) {
	    // We have a delayed close.
	    connect( socket, SIGNAL(delayedCloseFinished()), SLOT(socketClosed()) );
	} else {
	    // The socket is closed.
	    socketClosed();
	}
    }

    void sendToServer()
    {
	qWarning(QString("Client::sendToServer()"));
	// write to the server
	// 建立一個QTextStream其IODevice是socket
	QTextStream os(socket);
	// 把資料送到上面宣告的QTextStream
	os << inputText->text() << "\n";
	// 把輸入的視窗設定為空
	inputText->setText( "" );
    }

    void socketReadyRead()
    {
	qWarning(QString("Client::socketReadyRead()"));
	// read from the server
	// 判斷是否有資料可以讀取?
	while ( socket->canReadLine() ) {
	    // 由socket讀取一行字串並且把它送到infoText顯示在client端的視窗
	    infoText->append( socket->readLine() );
	}
    }

    void socketConnected()
    {
	qWarning(QString("Client::socketConnected()"));
	infoText->append( tr("Connected to server\n") );
    }

    void socketConnectionClosed()
    {
	qWarning(QString("Client::socketConnectionClosed()"));
	infoText->append( tr("Connection closed by the server\n") );
    }

    void socketClosed()
    {
	qWarning(QString("Client::socketClosed()"));
	infoText->append( tr("Connection closed\n") );
    }

    void socketError( int e )
    {
	qWarning(QString("Client::socketError(%1)").arg(e));
	infoText->append( tr("Error number %1 occurred\n").arg(e) );
    }

private:
    // 儲放server的socket物件
    QSocket *socket;
    // 表示要顯示在client的字串
    QTextView *infoText;
    // 要傳送到server端物件
    QLineEdit *inputText;
};


int main( int argc, char** argv )
{
    QApplication app( argc, argv );
    Client client( argc<2 ? "localhost" : argv[1], 4242 );
    app.setMainWidget( &client );
    client.show();
    return app.exec();
}

// 不知道下面這一行是什麼意思
#include "client.moc"
