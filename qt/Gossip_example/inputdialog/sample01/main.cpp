#include <qapplication.h>
#include <qlineedit.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

#include <sstream>
#include <string>
using namespace std;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    bool ok;

    QString text = QInputDialog::getText("Input text",
                   "please input a text", QLineEdit::Normal, "default", &ok);
    if(ok)
        QMessageBox::information(0, "QMessageBox::information",
            "Your input is: <u>" + text + "</u>",
             "OK", "Cancel", "Help", 0, 1);

    int integer = QInputDialog::getInteger("Input integer",
    	"enter a integer",0,0,100,1,&ok);

    if(ok) {
	string strl;
	stringstream sstr;
	sstr << integer;
	sstr >> strl;
        QMessageBox::information(0, "QMessageBox::information",
            "Your input is: <u>" + strl + "</u>",
             "OK", "Cancel", "Help", 0, 1);

    }

    double double_number = QInputDialog::getDouble("Input double",
    	"enter a double from 0.0 to 10.0",0.0,0.0,10.0,2,&ok);

    if (ok){
    	string strl;
	stringstream sstr;
	sstr << double_number;
	sstr >> strl;
	QMessageBox::information(0, "QMessageBox::information",
		"Your input is: <u>" + strl + "</u>",
		"OK", "Cancel", "Help",0,1);
    }

    QMessageBox::warning(0,"QMessageBox::warning",
    	"a warning messagebox!","ok","quit",0,0,1);

    QMessageBox::critical(0,"QMessageBox::critical",
    	"a critical messagebox!","ok","quit",0,0,1);

    QMessageBox::about(0,"MessageBox::about",
    	"This is caterpillar's qt tutorial!");

    switch(QMessageBox::warning(0,"Application name",
    	"Could not connect to the <mumble> server.\n"
	"This program can't function correctly"
	"without the server.\n\n",
	"Retry",
	"Quit",0,0,1)){
    case 0:
	QMessageBox::information(0,"QMessageBox::information",
		"Your press first button",
		QMessageBox::Ok);
	break;
    case 1:
	QMessageBox::information(0,"QMessageBox::information",
		"Your press second button",
		QMessageBox::Ok);
    	break;
    }

    return 0;
}
