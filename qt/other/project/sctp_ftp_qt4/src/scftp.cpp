#include <QtGui> 
#include "scftp.h"
extern "C" { 
#include "stream_common.h" 
}

ScFtp::ScFtp(QWidget *parent)
{
    setupUi(this); // this sets up GUI
 
    // signals/slots mechanism in action
    connect( btnGet, SIGNAL( clicked() ), this, SLOT( getPath() ) ); 
    connect( btnSend, SIGNAL( clicked() ), this, SLOT( send() ) ); 
    connect( btnReceive, SIGNAL( clicked() ), this, SLOT( receive() ) ); 
    connect( btnClear, SIGNAL( clicked() ), this, SLOT( clear() ) ); 
    connect( btnAbout, SIGNAL( clicked() ), this, SLOT( about() ) ); 
}


void ScFtp::getPath(){
    QString path;
    
    path = QFileDialog::getOpenFileName(
        this,
        "Choose a file to open",
        QString::null,
        QString::null);
 
    txtSourceFile->setText( path );
}


void ScFtp::send(){
  int inputStreams, outputStreams, several_parts;
  QByteArray tempIP, tempSrc;
  char *serverIP, *sourceFile;

  tempIP = txtServerIP->text().toLatin1();
  serverIP   = tempIP.data();
  tempSrc = txtSourceFile->text().toLatin1();
  sourceFile = tempSrc.data();
  txtOutput->append( "Path to file: " + QString(QLatin1String(serverIP)) );
  txtOutput->append( "Path to file: " + QString(QLatin1String(sourceFile)) );
  
  inputStreams = txtInputStreams->text().toInt();
  outputStreams = txtOutputStreams->text().toInt(); 
  several_parts = txtSeveralParts->text().toInt();
  txtOutput->append( "Output Streams: " + QString::number(inputStreams) );
  txtOutput->append( "Input Streams: " + QString::number(outputStreams) );
  txtOutput->append( "Several Parts: " + QString::number(several_parts) );
  
  sendFile(serverIP, inputStreams, outputStreams, sourceFile, several_parts);
}


void ScFtp::receive(){
  int inputStreams, outputStreams;

  inputStreams = txtInputStreams->text().toInt();
  outputStreams = txtOutputStreams->text().toInt(); 
  txtOutput->append( "Output Streams: " + QString::number(inputStreams) );
  txtOutput->append( "Input Streams: " + QString::number(outputStreams) );

  receiveFile(inputStreams, outputStreams);
}

void ScFtp::clear(){
  txtOutput->clear();
}
 
 
void ScFtp::about() {
    QMessageBox::about(this,"About SCTP File Transfer",
                "This app was coded for transfer files using SCTP protocol.\n\n"
                "AuthoR: Esteban Gutierrez Mlot \n"
                "License: GPL. \n");
}
