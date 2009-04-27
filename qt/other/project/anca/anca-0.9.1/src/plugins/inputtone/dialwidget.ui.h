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


void DialWidget::button1_clicked()
{
	emit sendTone('1');
}


void DialWidget::button2_clicked()
{
	emit sendTone('2');
}


void DialWidget::button3_clicked()
{
	emit sendTone('3');
}


void DialWidget::button4_clicked()
{
	emit sendTone('4');
}


void DialWidget::button5_clicked()
{
	emit sendTone('5');
}


void DialWidget::button6_clicked()
{
	emit sendTone('6');
}


void DialWidget::button7_clicked()
{
	emit sendTone('7');
}


void DialWidget::button8_clicked()
{
	emit sendTone('8');
}


void DialWidget::button9_clicked()
{
	emit sendTone('9');
}


void DialWidget::buttono_clicked()
{
	emit sendTone('*');
}


void DialWidget::button0_clicked()
{
	emit sendTone('0');
}


void DialWidget::buttonx_clicked()
{
	emit sendTone('!');
}
