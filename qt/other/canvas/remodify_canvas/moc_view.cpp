/****************************************************************************
** View meta object code from reading C++ file 'view.h'
**
** Created: Wed Sep 3 12:23:24 2008
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.7   edited Oct 19 16:22 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "view.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *View::className() const
{
    return "View";
}

QMetaObject *View::metaObj = 0;
static QMetaObjectCleanUp cleanUp_View( "View", &View::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString View::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "View", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString View::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "View", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* View::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvasView::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"View", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_View.setMetaObject( metaObj );
    return metaObj;
}

void* View::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "View" ) )
	return this;
    return QCanvasView::qt_cast( clname );
}

bool View::qt_invoke( int _id, QUObject* _o )
{
    return QCanvasView::qt_invoke(_id,_o);
}

bool View::qt_emit( int _id, QUObject* _o )
{
    return QCanvasView::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool View::qt_property( int id, int f, QVariant* v)
{
    return QCanvasView::qt_property( id, f, v);
}

bool View::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
