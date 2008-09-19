
#include <qmessagebox.h>

#include "warnqvbox.h"

void warnQVBox::warnmessage()
{
	QMessageBox::warning(this,"caption","message",
		QMessageBox::Yes | QMessageBox::Default,
		QMessageBox::No,
		QMessageBox::Cancel | QMessageBox::Escape);
}
