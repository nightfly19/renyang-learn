#include <qptrdict.h>
#include <qlineedit.h>
#include <qapplication.h>

int main(int argc,char **argv)
{
	QApplication app(argc,argv);

	QPtrDict<char> extra;

	QLineEdit *le1 = new QLineEdit(0);
	le1->setText("Simpson");
	QLineEdit *le2 = new QLineEdit(0);
	le2->setText("Homer");
	QLineEdit *le3 = new QLineEdit(0);
	le3->setText("45");

	// void insert ( void * key, const type * item )
	extra.insert(le1,"Surname");
	extra.insert(le2,"Forename");
	extra.insert(le3,"Age");

	// 由key去找type
	// type * find ( void * key ) const
	qWarning("%s",extra.find(le1));
	
	return app.exec();
}
