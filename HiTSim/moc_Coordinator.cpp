/****************************************************************************
** Coordinator meta object code from reading C++ file 'Coordinator.h'
**
** Created: Wed Oct 11 22:47:32 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "Coordinator.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Coordinator::className() const
{
    return "Coordinator";
}

QMetaObject *Coordinator::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Coordinator( "Coordinator", &Coordinator::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Coordinator::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Coordinator", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Coordinator::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Coordinator", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Coordinator::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUMethod slot_0 = {"active", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "active()", &slot_0, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"Coordinator", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Coordinator.setMetaObject( metaObj );
    return metaObj;
}

void* Coordinator::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Coordinator" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool Coordinator::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: active(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Coordinator::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Coordinator::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool Coordinator::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
