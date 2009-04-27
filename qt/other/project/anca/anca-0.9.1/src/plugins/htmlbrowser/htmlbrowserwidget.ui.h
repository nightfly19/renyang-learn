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

#include <qurloperator.h>
#include <qnetwork.h>

#include "netmime.h"

QUrlOperator *urlOperator = 0;

void HTMLBrowserWidget::goButton_clicked()
{
	htmlBrowser->setSource( sourceEdit->text() );
}


void HTMLBrowserWidget::sourceEdit_returnPressed()
{
	goButton_clicked();
}

