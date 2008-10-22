#ifndef ARTISTCOMBOBOX_H
#define ARTISTCOMBOBOX_H

#include <qcombobox.h>
#include <qmap.h>

class QSqlDatabase;

class ArtistComboBox : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(int artistId READ artistId WRITE setArtistId)
public:
    ArtistComboBox(QSqlDatabase *database, QWidget *parent = 0,
                   const char *name = 0);

    void refresh();
    int artistId() const;
    void setArtistId(int id);

private:
    void populate();

    QSqlDatabase *db;
    QMap<int, int> idFromIndex;
    QMap<int, int> indexFromId;
};

#endif
