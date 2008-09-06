#ifndef ARTISTFORM_H
#define ARTISTFORM_H

#include <qdialog.h>

class QDataTable;
class QPushButton;
class QSqlDatabase;
class QSqlRecord;

class ArtistForm : public QDialog
{
    Q_OBJECT
public:
    ArtistForm(QWidget *parent = 0, const char *name = 0);

protected slots:
    void accept();
    void reject();

private slots:
    void primeInsertArtist(QSqlRecord *buffer);
    void beforeInsertArtist(QSqlRecord *buffer);
    void beforeDeleteArtist(QSqlRecord *buffer);

private:
    QSqlDatabase *db;
    QDataTable *artistTable;
    QPushButton *updateButton;
    QPushButton *cancelButton;
};

#endif
