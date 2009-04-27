/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

/**
 * @class AnswerCallDlg
 * @short Dialog that is shown when incoming call appeares.
 *
 * It has a timer that counts down the timeout. If the timer
 * comes to zero, the dialog is rejected and timeouted flag is set.
 */

void AnswerCallDlg::show()
{
	QDialog::show();
	timeouted = false;

	//start timer if requested
	if( m_timeout > 0 ) {
		currentTime = m_timeout;
		setTimeoutLabel();
		startTimer(1000);
	}
	else
		timeoutLabel->hide();
}


void AnswerCallDlg::setTimeout( int timeout )
{
	m_timeout = timeout;
}


void AnswerCallDlg::timerEvent( QTimerEvent * )
{
	currentTime--;
	setTimeoutLabel();
	if( !currentTime ) {
		timeouted = true;
		killTimers();
		reject();
	}
}


bool AnswerCallDlg::wasTimeout()
{
	return timeouted;
}


void AnswerCallDlg::setTimeoutLabel()
{
	char buf[6];
	int min = currentTime / 60;
	int sec = currentTime % 60;
	sprintf(buf, "%02d:%02d", min, sec);
	timeoutLabel->setText(buf);
}
