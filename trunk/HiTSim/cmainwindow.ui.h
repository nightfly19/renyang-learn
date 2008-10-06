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

#include <qapplication.h>

void cMainWindow::fileNew()
{

}


void cMainWindow::fileOpen()
{

}


void cMainWindow::fileSave()
{

}


void cMainWindow::fileSaveAs()
{

}


void cMainWindow::filePrint()
{

}


void cMainWindow::fileExit()
{
    qApp->quit();
}


void cMainWindow::helpIndex()
{

}


void cMainWindow::helpContents()
{

}


void cMainWindow::helpAbout()
{

}


void cMainWindow::slotClear()
{

}


void cMainWindow::slotAddSprite()
{

}


void cMainWindow::slotNewView()
{

}


void cMainWindow::toggleDoubleBuffer(bool)
{

}


void cMainWindow::slotAddCar()
{

}


void cMainWindow::slotRandAddWifiCars()
{

}


void cMainWindow::slotEnlarge()
{

}


void cMainWindow::slotShrink()
{

}


void cMainWindow::slotZoomIn()
{

}


void cMainWindow::slotZoomOut()
{

}

void cMainWindow::slotSetTimeRatio()
{
    
}


void cMainWindow::slotStop()
{
	actPlay->setOn(false);
//	actStop->setOn(false);
	actPause->setOn(false);
}


void cMainWindow::slotPlay()
{
//      actPlay->setOn(false);
	actStop->setOn(false);
	actPause->setOn(false); 
}


void cMainWindow::slotPause()
{
	actPlay->setOn(false);
	actStop->setOn(false);
//	actPause->setOn(false); 

}


void cMainWindow::slotActPosViewer( bool )
{

}


void cMainWindow::slotActCenter()
{

}


void cMainWindow::slotActUsrSel()
{

}


void cMainWindow::slotActSimSet()
{

}


void cMainWindow::slotRandAddDSRCCar()
{

}


void cMainWindow::slotLogFileSetting()
{

}


void cMainWindow::slotFileOpen()
{

}
