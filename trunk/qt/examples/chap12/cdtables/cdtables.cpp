#include <qapplication.h>
#include <qsqldatabase.h>

#include "../cdcollection/connection.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (!createConnections())
        return 1;

    QSqlQuery query;
    query.exec("DROP TABLE track");
    query.exec("DROP TABLE cd");
    query.exec("DROP TABLE artist");
    query.exec("CREATE TABLE artist ("
               "id INTEGER PRIMARY KEY, "
               "name VARCHAR(40) NOT NULL, "
               "country VARCHAR(40))");
    query.exec("CREATE TABLE cd ("
               "id INTEGER PRIMARY KEY, "
               "artistid INTEGER NOT NULL, "
               "title VARCHAR(40) NOT NULL, "
               "year INTEGER NOT NULL)");
    query.exec("CREATE TABLE track ("
               "id INTEGER PRIMARY KEY, "
               "cdid INTEGER NOT NULL, "
               "number INTEGER NOT NULL, "
               "title VARCHAR(40) NOT NULL, "
               "duration INTEGER NOT NULL)");

#if 1
    query.exec("INSERT INTO artist "
               "VALUES (1001, 'Gluecifer', 'Norway')");
    query.exec("INSERT INTO artist "
               "VALUES (1002, 'The Sounds', 'Sweden')");
    query.exec("INSERT INTO artist "
               "VALUES (1003, 'Melvins', 'USA')");
    query.exec("INSERT INTO artist "
               "VALUES (1004, 'Strapping Young Lads', 'Canada')");
    query.exec("INSERT INTO cd "
               "VALUES (2001, 1001, 'Riding the Tiger', 1997)");
    query.exec("INSERT INTO cd "
               "VALUES (2002, 1001, 'Easy Living', 2002)");
    query.exec("INSERT INTO cd "
               "VALUES (2003, 1002, 'Living in America', 2002)");
    query.exec("INSERT INTO cd "
               "VALUES (2004, 1003, 'Bullhead', 1991)");
    query.exec("INSERT INTO cd "
               "VALUES (2005, 1003, 'Stag', 1996)");
    query.exec("INSERT INTO cd "
               "VALUES (2006, 1003, 'Hostile Ambient Takeover', "
               "2002)");
    query.exec("INSERT INTO cd "
               "VALUES (2007, 1003, '26 Songs', 2003)");
    query.exec("INSERT INTO cd "
               "VALUES (2008, 1004, 'City', 1997)");
    query.exec("INSERT INTO cd "
               "VALUES (2009, 1004, 'No Sleep Till Bedtime', "
               "1998)");
    query.exec("INSERT INTO cd "
               "VALUES (2010, 1004, 'Syl', 2003)");
    query.exec("INSERT INTO track "
               "VALUES (3001, 2004, 01, 'Boris', 514)");
    query.exec("INSERT INTO track "
               "VALUES (3002, 2004, 02, 'Anaconda', 143)");
    query.exec("INSERT INTO track "
               "VALUES (3003, 2004, 03, 'Ligature', 229)");
    query.exec("INSERT INTO track "
               "VALUES (3004, 2004, 04, 'It''s Shoved', 155)");
    query.exec("INSERT INTO track "
               "VALUES (3005, 2004, 05, 'Zodiac', 254)");
    query.exec("INSERT INTO track "
               "VALUES (3006, 2004, 06, 'If I Had An Exorcism', "
               "187)");
    query.exec("INSERT INTO track "
               "VALUES (3007, 2004, 07, 'Your Blessed', 339)");
    query.exec("INSERT INTO track "
               "VALUES (3008, 2004, 08, 'Cow', 271)");
#endif

    return 0;
}
