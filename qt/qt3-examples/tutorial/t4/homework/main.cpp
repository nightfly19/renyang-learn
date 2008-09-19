
/****************************************************************
**
** Qt - 4
**
****************************************************************/

#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>


class MyWidget : public QWidget
{
public:
    MyWidget( QWidget *parent=0, const char *name=0 );
};


MyWidget::MyWidget( QWidget *parent, const char *name )
        : QWidget( parent, name )
{
    setMinimumSize( 200, 120 );
    setMaximumSize( 200, 120 );

    QPushButton *quit = new QPushButton( "Quit", this, "quit" );
    quit->setGeometry( 62, 40, 75, 30 );
    quit->setFont( QFont( "Times", 18, QFont::Bold ) );

    QPushButton *show = new QPushButton("Show",this,"show");
    show->setGeometry(43,54,75,30);
    show->setFont(QFont("Times",18,QFont::Bold));

    connect( quit, SIGNAL(clicked()), qApp, SLOT(quit()) );
}


int main( int argc, char **argv )
{
    QApplication a( argc, argv );

    MyWidget w,p; 
    w.setGeometry( 100, 100, 200, 120 );
    p.setGeometry(200,300,200,120);
    p.show();
    a.setMainWidget( &w );
    w.show();
    return a.exec();
}
