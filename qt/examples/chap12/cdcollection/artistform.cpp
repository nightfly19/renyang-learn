#include <qdatatable.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qsqldatabase.h>
#include <qsqlrecord.h>

#include "artistform.h"
#include "generateid.h"

ArtistForm::ArtistForm(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Update Artists"));

    db = QSqlDatabase::database("ARTIST");
    db->transaction();

    QSqlCursor *artistCursor = new QSqlCursor("artist", true, db);
    artistTable = new QDataTable(artistCursor, false, this);
    artistTable->addColumn("name", tr("Name"));
    artistTable->addColumn("country", tr("Country"));
    artistTable->setAutoDelete(true);
    artistTable->setConfirmDelete(true);
    artistTable->setSorting(true);
    artistTable->refresh();

    updateButton = new QPushButton(tr("Update"), this);
    updateButton->setDefault(true);
    cancelButton = new QPushButton(tr("Cancel"), this);

    connect(artistTable, SIGNAL(beforeDelete(QSqlRecord *)),
            this, SLOT(beforeDeleteArtist(QSqlRecord *)));
    connect(artistTable, SIGNAL(primeInsert(QSqlRecord *)),
            this, SLOT(primeInsertArtist(QSqlRecord *)));
    connect(artistTable, SIGNAL(beforeInsert(QSqlRecord *)),
            this, SLOT(beforeInsertArtist(QSqlRecord *)));
    connect(updateButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(artistTable);
    mainLayout->addLayout(buttonLayout);
}

void ArtistForm::accept()
{
    db->commit();
    QDialog::accept();
}

void ArtistForm::reject()
{
    db->rollback();
    QDialog::reject();
}

void ArtistForm::beforeDeleteArtist(QSqlRecord *buffer)
{
    QSqlQuery query(db);
    query.exec("DELETE FROM track WHERE track.id IN "
               "(SELECT track.id FROM track, cd "
               "WHERE track.cdid = cd.id AND cd.artistid = "
               + buffer->value("id").toString() + ")");
    query.exec("DELETE FROM cd WHERE artistid = "
               + buffer->value("id").toString());
}

void ArtistForm::primeInsertArtist(QSqlRecord *buffer)
{
    buffer->setValue("country", "USA");
}

void ArtistForm::beforeInsertArtist(QSqlRecord *buffer)
{
    buffer->setValue("id", generateId("artist", db));
}
