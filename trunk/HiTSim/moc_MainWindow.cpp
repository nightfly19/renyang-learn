/****************************************************************************
** Main meta object code from reading C++ file 'MainWindow.h'
**
** Created: Fri Oct 20 00:38:21 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "MainWindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Main::className() const
{
    return "Main";
}

QMetaObject *Main::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Main( "Main", &Main::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Main::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Main", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Main::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Main", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Main::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = cMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"helpAbout", 0, 0 };
    static const QUMethod slot_1 = {"slotNewView", 0, 0 };
    static const QUMethod slot_2 = {"slotClear", 0, 0 };
    static const QUMethod slot_3 = {"init", 0, 0 };
    static const QUMethod slot_4 = {"slotAddSprite", 0, 0 };
    static const QUMethod slot_5 = {"slotAddCar", 0, 0 };
    static const QUMethod slot_6 = {"slotRandAddCars", 0, 0 };
    static const QUParameter param_slot_7[] = {
	{ 0, &static_QUType_ptr, "QCanvasItem", QUParameter::In }
    };
    static const QUMethod slot_7 = {"slotLeftSelect", 1, param_slot_7 };
    static const QUParameter param_slot_8[] = {
	{ 0, &static_QUType_ptr, "QCanvasItem", QUParameter::In }
    };
    static const QUMethod slot_8 = {"slotRightSelect", 1, param_slot_8 };
    static const QUMethod slot_9 = {"slotMouseRelease", 0, 0 };
    static const QUMethod slot_10 = {"slotEnlarge", 0, 0 };
    static const QUMethod slot_11 = {"slotShrink", 0, 0 };
    static const QUMethod slot_12 = {"slotZoomIn", 0, 0 };
    static const QUMethod slot_13 = {"slotZoomOut", 0, 0 };
    static const QUMethod slot_14 = {"rotateClockwise", 0, 0 };
    static const QUMethod slot_15 = {"rotateCounterClockwise", 0, 0 };
    static const QUMethod slot_16 = {"mirror", 0, 0 };
    static const QUMethod slot_17 = {"moveL", 0, 0 };
    static const QUMethod slot_18 = {"moveR", 0, 0 };
    static const QUMethod slot_19 = {"moveU", 0, 0 };
    static const QUMethod slot_20 = {"moveD", 0, 0 };
    static const QUMethod slot_21 = {"print", 0, 0 };
    static const QUParameter param_slot_22[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_22 = {"toggleDoubleBuffer", 1, param_slot_22 };
    static const QMetaData slot_tbl[] = {
	{ "helpAbout()", &slot_0, QMetaData::Public },
	{ "slotNewView()", &slot_1, QMetaData::Private },
	{ "slotClear()", &slot_2, QMetaData::Private },
	{ "init()", &slot_3, QMetaData::Private },
	{ "slotAddSprite()", &slot_4, QMetaData::Private },
	{ "slotAddCar()", &slot_5, QMetaData::Private },
	{ "slotRandAddCars()", &slot_6, QMetaData::Private },
	{ "slotLeftSelect(QCanvasItem*)", &slot_7, QMetaData::Private },
	{ "slotRightSelect(QCanvasItem*)", &slot_8, QMetaData::Private },
	{ "slotMouseRelease()", &slot_9, QMetaData::Private },
	{ "slotEnlarge()", &slot_10, QMetaData::Private },
	{ "slotShrink()", &slot_11, QMetaData::Private },
	{ "slotZoomIn()", &slot_12, QMetaData::Private },
	{ "slotZoomOut()", &slot_13, QMetaData::Private },
	{ "rotateClockwise()", &slot_14, QMetaData::Private },
	{ "rotateCounterClockwise()", &slot_15, QMetaData::Private },
	{ "mirror()", &slot_16, QMetaData::Private },
	{ "moveL()", &slot_17, QMetaData::Private },
	{ "moveR()", &slot_18, QMetaData::Private },
	{ "moveU()", &slot_19, QMetaData::Private },
	{ "moveD()", &slot_20, QMetaData::Private },
	{ "print()", &slot_21, QMetaData::Private },
	{ "toggleDoubleBuffer(bool)", &slot_22, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"Main", parentObject,
	slot_tbl, 23,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Main.setMetaObject( metaObj );
    return metaObj;
}

void* Main::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Main" ) )
	return this;
    return cMainWindow::qt_cast( clname );
}

bool Main::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: helpAbout(); break;
    case 1: slotNewView(); break;
    case 2: slotClear(); break;
    case 3: init(); break;
    case 4: slotAddSprite(); break;
    case 5: slotAddCar(); break;
    case 6: slotRandAddCars(); break;
    case 7: slotLeftSelect((QCanvasItem*)static_QUType_ptr.get(_o+1)); break;
    case 8: slotRightSelect((QCanvasItem*)static_QUType_ptr.get(_o+1)); break;
    case 9: slotMouseRelease(); break;
    case 10: slotEnlarge(); break;
    case 11: slotShrink(); break;
    case 12: slotZoomIn(); break;
    case 13: slotZoomOut(); break;
    case 14: rotateClockwise(); break;
    case 15: rotateCounterClockwise(); break;
    case 16: mirror(); break;
    case 17: moveL(); break;
    case 18: moveR(); break;
    case 19: moveU(); break;
    case 20: moveD(); break;
    case 21: print(); break;
    case 22: toggleDoubleBuffer((bool)static_QUType_bool.get(_o+1)); break;
    default:
	return cMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Main::qt_emit( int _id, QUObject* _o )
{
    return cMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Main::qt_property( int id, int f, QVariant* v)
{
    return cMainWindow::qt_property( id, f, v);
}

bool Main::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
