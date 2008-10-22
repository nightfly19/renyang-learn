#include <qsqlcursor.h>
#include <qsqldatabase.h>

#include "artistcombobox.h"

ArtistComboBox::ArtistComboBox(QSqlDatabase *database,
                               QWidget *parent, const char *name)
    : QComboBox(parent, name)
{
    db = database;
    populate();
}

void ArtistComboBox::refresh()
{
    int oldArtistId = artistId();
    clear();
    idFromIndex.clear();
    indexFromId.clear();
    populate();
    setArtistId(oldArtistId);
}

void ArtistComboBox::populate()
{
    QSqlCursor cursor("artist", true, db);
    cursor.select(cursor.index("name"));

    int index = 0;
    while (cursor.next()) {
        int id = cursor.value("id").toInt();
        insertItem(cursor.value("name").toString(), index);
        idFromIndex[index] = id;
        indexFromId[id] = index;
        ++index;
    }
}

int ArtistComboBox::artistId() const
{
    return idFromIndex[currentItem()];
}

void ArtistComboBox::setArtistId(int id)
{
    if (indexFromId.contains(id))
        setCurrentItem(indexFromId[id]);
}
