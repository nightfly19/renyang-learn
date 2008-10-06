/****************************************************************************
** FigureEditor meta object code from reading C++ file 'FigureEditor.h'
**
** Created: Wed Oct 11 22:47:32 2006
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.6   edited Mar 8 17:43 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "FigureEditor.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *FigureEditor::className() const
{
    return "FigureEditor";
}

QMetaObject *FigureEditor::metaObj = 0;
static QMetaObjectCleanUp cleanUp_FigureEditor( "FigureEditor", &FigureEditor::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString FigureEditor::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FigureEditor", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString FigureEditor::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FigureEditor", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* FigureEditor::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvasView::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"status", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_ptr, "QCanvasItem", QUParameter::In }
    };
    static const QUMethod signal_1 = {"leftSelect", 1, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_ptr, "QCanvasItem", QUParameter::In }
    };
    static const QUMethod signal_2 = {"rightSelect", 1, param_signal_2 };
    static const QUMethod signal_3 = {"sigMouseRelease", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "status(const QString&)", &signal_0, QMetaData::Protected },
	{ "leftSelect(QCanvasItem*)", &signal_1, QMetaData::Protected },
	{ "rightSelect(QCanvasItem*)", &signal_2, QMetaData::Protected },
	{ "sigMouseRelease()", &signal_3, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"FigureEditor", parentObject,
	0, 0,
	signal_tbl, 4,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_FigureEditor.setMetaObject( metaObj );
    return metaObj;
}

void* FigureEditor::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "FigureEditor" ) )
	return this;
    return QCanvasView::qt_cast( clname );
}

// SIGNAL status
void FigureEditor::status( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL leftSelect
void FigureEditor::leftSelect( QCanvasItem* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 1 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL rightSelect
void FigureEditor::rightSelect( QCanvasItem* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL sigMouseRelease
void FigureEditor::sigMouseRelease()
{
    activate_signal( staticMetaObject()->signalOffset() + 3 );
}

bool FigureEditor::qt_invoke( int _id, QUObject* _o )
{
    return QCanvasView::qt_invoke(_id,_o);
}

bool FigureEditor::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: status((const QString&)static_QUType_QString.get(_o+1)); break;
    case 1: leftSelect((QCanvasItem*)static_QUType_ptr.get(_o+1)); break;
    case 2: rightSelect((QCanvasItem*)static_QUType_ptr.get(_o+1)); break;
    case 3: sigMouseRelease(); break;
    default:
	return QCanvasView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool FigureEditor::qt_property( int id, int f, QVariant* v)
{
    return QCanvasView::qt_property( id, f, v);
}

bool FigureEditor::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
