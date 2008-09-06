
/****************************************************************
**
** Qt tutorial 8
**
****************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qlcdnumber.h>
#include <qfont.h>
#include <qlayout.h>

#include "lcdrange.h"
#include "cannon.h"


class MyWidget: public QWidget
{
public:
    MyWidget( QWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    QPushButton *quit = new QPushButton( "Quit", this, "quit" );
    quit->setFont( QFont( "Times", 18, QFont::Bold ) );

    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );

    QPushButton *move_button = new QPushButton("Move",this,"move");
    move_button->setFont(QFont("Times",18,QFont::Bold));

    LCDRange *angle = new LCDRange( this, "angle" );
    angle->setRange( 5, 70 );

    CannonField *cannonField
        = new CannonField( this, "cannonField" );

    connect(move_button,SIGNAL(clicked()),cannonField,SLOT(move_circle()));

    connect( angle, SIGNAL(valueChanged(int)),
             cannonField, SLOT(setAngle(int)) );
    connect( cannonField, SIGNAL(angleChanged(int)),
             angle, SLOT(setValue(int)) );

    QGridLayout *grid = new QGridLayout( this, 2, 2, 10 );
    //2x2, 10 pixel border

    grid->addWidget( quit, 0, 0 );
    grid->addWidget( angle, 1, 0, Qt::AlignTop );
    grid->addWidget( cannonField, 1, 1 );
    grid->addWidget(move_button,0,1);
    grid->setColStretch( 1, 10 );

    angle->setValue( 60 );
    angle->setFocus();
}


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MyWidget w;
    w.setGeometry( 100, 100, 500, 355 );
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
