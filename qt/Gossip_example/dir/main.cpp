#include <stdio.h>
#include <qdir.h>

int main( int argc, char **argv )
{
    QDir d;
    d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
    d.setSorting( QDir::Size | QDir::Reversed );

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );
    QFileInfo *fi;

    printf( "     Bytes Filename\n" );
    while ( (fi = it.current()) != 0 ) {
        printf( "%10li %s\n", fi->size(), fi->fileName().latin1() );
        ++it;
    }
    return 0;
}
