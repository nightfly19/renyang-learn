#ifndef SCFTP_H
#define SCFTP_H
 
#include "ui_scftp.h"
 
class ScFtp:public QWidget, private Ui::ScFtpDLG{
    Q_OBJECT
 
public:
    ScFtp(QWidget *parent = 0);
 
 
public slots:
    void getPath();
    void send();
    void receive();
    void clear();
    void about();
};
 
 
#endif
