/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

#include <qspinbox.h>
#include <qcanvas.h>

#include "Car.h"

void DlgCarSetting::setValue( int v1 , int v2 )
{
	sp_velocity -> setValue( v1 );
	sp_max_velocity -> setValue( v2 );
}
