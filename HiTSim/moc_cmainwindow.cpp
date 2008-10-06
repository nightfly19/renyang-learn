/****************************************************************************
** cMainWindow meta object code from reading C++ file 'cmainwindow.h'
**
** Created: Fri Oct 20 00:38:22 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "cmainwindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *cMainWindow::className() const
{
    return "cMainWindow";
}

QMetaObject *cMainWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_cMainWindow( "cMainWindow", &cMainWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString cMainWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "cMainWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString cMainWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "cMainWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* cMainWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"fileNew", 0, 0 };
    static const QUMethod slot_1 = {"fileOpen", 0, 0 };
    static const QUMethod slot_2 = {"fileSave", 0, 0 };
    static const QUMethod slot_3 = {"fileSaveAs", 0, 0 };
    static const QUMethod slot_4 = {"filePrint", 0, 0 };
    static const QUMethod slot_5 = {"fileExit", 0, 0 };
    static const QUMethod slot_6 = {"helpIndex", 0, 0 };
    static const QUMethod slot_7 = {"helpContents", 0, 0 };
    static const QUMethod slot_8 = {"helpAbout", 0, 0 };
    static const QUMethod slot_9 = {"slotClear", 0, 0 };
    static const QUMethod slot_10 = {"slotAddSprite", 0, 0 };
    static const QUMethod slot_11 = {"slotNewView", 0, 0 };
    static const QUParameter param_slot_12[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_12 = {"toggleDoubleBuffer", 1, param_slot_12 };
    static const QUMethod slot_13 = {"slotAddCar", 0, 0 };
    static const QUMethod slot_14 = {"slotRandAddCars", 0, 0 };
    static const QUMethod slot_15 = {"slotEnlarge", 0, 0 };
    static const QUMethod slot_16 = {"slotShrink", 0, 0 };
    static const QUMethod slot_17 = {"slotZoomIn", 0, 0 };
    static const QUMethod slot_18 = {"slotZoomOut", 0, 0 };
    static const QUMethod slot_19 = {"languageChange", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "fileNew()", &slot_0, QMetaData::Public },
	{ "fileOpen()", &slot_1, QMetaData::Public },
	{ "fileSave()", &slot_2, QMetaData::Public },
	{ "fileSaveAs()", &slot_3, QMetaData::Public },
	{ "filePrint()", &slot_4, QMetaData::Public },
	{ "fileExit()", &slot_5, QMetaData::Public },
	{ "helpIndex()", &slot_6, QMetaData::Public },
	{ "helpContents()", &slot_7, QMetaData::Public },
	{ "helpAbout()", &slot_8, QMetaData::Public },
	{ "slotClear()", &slot_9, QMetaData::Public },
	{ "slotAddSprite()", &slot_10, QMetaData::Public },
	{ "slotNewView()", &slot_11, QMetaData::Public },
	{ "toggleDoubleBuffer(bool)", &slot_12, QMetaData::Public },
	{ "slotAddCar()", &slot_13, QMetaData::Public },
	{ "slotRandAddCars()", &slot_14, QMetaData::Public },
	{ "slotEnlarge()", &slot_15, QMetaData::Public },
	{ "slotShrink()", &slot_16, QMetaData::Public },
	{ "slotZoomIn()", &slot_17, QMetaData::Public },
	{ "slotZoomOut()", &slot_18, QMetaData::Public },
	{ "languageChange()", &slot_19, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"cMainWindow", parentObject,
	slot_tbl, 20,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_cMainWindow.setMetaObject( metaObj );
    return metaObj;
}

void* cMainWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "cMainWindow" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool cMainWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: fileNew(); break;
    case 1: fileOpen(); break;
    case 2: fileSave(); break;
    case 3: fileSaveAs(); break;
    case 4: filePrint(); break;
    case 5: fileExit(); break;
    case 6: helpIndex(); break;
    case 7: helpContents(); break;
    case 8: helpAbout(); break;
    case 9: slotClear(); break;
    case 10: slotAddSprite(); break;
    case 11: slotNewView(); break;
    case 12: toggleDoubleBuffer((bool)static_QUType_bool.get(_o+1)); break;
    case 13: slotAddCar(); break;
    case 14: slotRandAddCars(); break;
    case 15: slotEnlarge(); break;
    case 16: slotShrink(); break;
    case 17: slotZoomIn(); break;
    case 18: slotZoomOut(); break;
    case 19: languageChange(); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool cMainWindow::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool cMainWindow::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool cMainWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
