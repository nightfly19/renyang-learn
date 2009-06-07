#include <qstring.h>
#include <qmap.h>
#include <qstring.h>
#include <qapplication.h>

class Employee
{
public:
	Employee(): sn(0) {}
	Employee( const QString& forename, const QString& surname, int salary ) : fn(forename), sn(surname), sal(salary)
	{ }

	QString forename() const { return fn; }
	QString surname() const { return sn; }
	int salary() const { return sal; }
	void setSalary( int salary ) { sal = salary; }

private:
	QString fn;
	QString sn;
	int sal;
};

int main(int argc, char **argv)
{
	QApplication app( argc, argv );

	typedef QMap<QString, Employee> EmployeeMap;
	EmployeeMap map;

	map["JD001"] = Employee("John", "Doe", 50000);
	map["JD002"] = Employee("Jane", "Williams", 80000);
	map["TJ001"] = Employee("Tom", "Jones", 60000);

	Employee sasha( "Sasha", "Hind", 50000 );
	map["SH001"] = sasha;
	sasha.setSalary( 40000 );

	EmployeeMap::Iterator it;
	for ( it = map.begin(); it != map.end(); ++it ) {
	printf( "%s: %s, %s earns %d\n",it.key().latin1(),it.data().surname().latin1(),it.data().forename().latin1(),it.data().salary() );
	}

	// 透過find去尋找相對應的值
	it = map.find("JD001");
	printf( "%s: %s, %s earns %d\n",it.key().latin1(),it.data().surname().latin1(),it.data().forename().latin1(),it.data().salary() );
	return 0;
}
