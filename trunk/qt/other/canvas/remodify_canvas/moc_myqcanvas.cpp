/****************************************************************************
** MyQCanvas meta object code from reading C++ file 'myqcanvas.h'
**
** Created: Wed Sep 3 12:23:23 2008
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.7   edited Oct 19 16:22 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "myqcanvas.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MyQCanvas::className() const
{
    return "MyQCanvas";
}

QMetaObject *MyQCanvas::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MyQCanvas( "MyQCanvas", &MyQCanvas::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MyQCanvas::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyQCanvas", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MyQCanvas::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyQCanvas", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MyQCanvas::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvas::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"MyQCanvas", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MyQCanvas.setMetaObject( metaObj );
    return metaObj;
}

void* MyQCanvas::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MyQCanvas" ) )
	return this;
    return QCanvas::qt_cast( clname );
}

bool MyQCanvas::qt_invoke( int _id, QUObject* _o )
{
    return QCanvas::qt_invoke(_id,_o);
}

bool MyQCanvas::qt_emit( int _id, QUObject* _o )
{
    return QCanvas::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MyQCanvas::qt_property( int id, int f, QVariant* v)
{
    return QCanvas::qt_property( id, f, v);
}

bool MyQCanvas::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
