#include <qdatatable.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qsqlcursor.h>
#include <qsqldatabase.h>
#include <qsqlform.h>
#include <qsqlpropertymap.h>

#include "artistcombobox.h"
#include "artistform.h"
#include "cdform.h"
#include "generateid.h"

CdForm::CdForm(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Add a CD"));
    cdId = -1;
    init();
}

CdForm::CdForm(int id, QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("Edit a CD"));
    cdId = id;
    init();
}

CdForm::~CdForm()
{
    delete cdCursor;
    delete trackCursor;
}

void CdForm::init()
{
    db = QSqlDatabase::database("CD");
    db->transaction();
    if (cdId == -1)
        createNewRecord();

    titleLabel = new QLabel(tr("&Title:"), this);
    artistLabel = new QLabel(tr("&Artist:"), this);
    yearLabel = new QLabel(tr("&Year:"), this);
    titleLineEdit = new QLineEdit(this);
    yearSpinBox = new QSpinBox(this);
    yearSpinBox->setRange(1900, 2100);
    yearSpinBox->setValue(QDate::currentDate().year());
    artistComboBox = new ArtistComboBox(db, this);
    artistButton = new QPushButton(tr("Add &New..."), this);
    moveUpButton = new QPushButton(tr("&Move Up"), this);
    moveDownButton = new QPushButton(tr("Move &Down"), this);
    updateButton = new QPushButton(tr("Update"), this);
    updateButton->setDefault(true);
    updateButton->setEnabled(artistComboBox->count() > 0);
    cancelButton = new QPushButton(tr("Cancel"), this);

    titleLabel->setBuddy(titleLineEdit);
    artistLabel->setBuddy(artistComboBox);
    yearLabel->setBuddy(yearSpinBox);

    trackCursor = new QSqlCursor("track", true, db);
    trackTable = new QDataTable(trackCursor, false, this);
    trackTable->setFilter("cdid = " + QString::number(cdId));
    trackTable->setSort(trackCursor->index("number"));
    trackTable->addColumn("title", tr("Track"));
    trackTable->addColumn("duration", tr("Duration"));
    trackTable->refresh();

    cdCursor = new QSqlCursor("cd", true, db);
    cdCursor->select("id = " + QString::number(cdId));
    cdCursor->next();

    QSqlPropertyMap *propertyMap = new QSqlPropertyMap;
    propertyMap->insert("ArtistComboBox", "artistId");

    sqlForm = new QSqlForm(this);
    sqlForm->installPropertyMap(propertyMap);
    sqlForm->setRecord(cdCursor->primeUpdate());
    sqlForm->insert(titleLineEdit, "title");
    sqlForm->insert(artistComboBox, "artistid");
    sqlForm->insert(yearSpinBox, "year");
    sqlForm->readFields();

    connect(artistButton, SIGNAL(clicked()),
            this, SLOT(addNewArtist()));
    connect(moveUpButton, SIGNAL(clicked()),
            this, SLOT(moveTrackUp()));
    connect(moveDownButton, SIGNAL(clicked()),
            this, SLOT(moveTrackDown()));
    connect(updateButton, SIGNAL(clicked()),
            this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),
            this, SLOT(reject()));
    connect(trackTable, SIGNAL(beforeInsert(QSqlRecord *)),
            this, SLOT(beforeInsertTrack(QSqlRecord *)));
    connect(trackTable, SIGNAL(beforeDelete(QSqlRecord *)),
            this, SLOT(beforeDeleteTrack(QSqlRecord *)));

    QVBoxLayout *rightButtonLayout = new QVBoxLayout;
    rightButtonLayout->addWidget(moveUpButton);
    rightButtonLayout->addWidget(moveDownButton);
    rightButtonLayout->addStretch(1);

    QHBoxLayout *bottomButtonLayout = new QHBoxLayout;
    bottomButtonLayout->addStretch(1);
    bottomButtonLayout->addWidget(updateButton);
    bottomButtonLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);

    mainLayout->addWidget(titleLabel, 0, 0);
    mainLayout->addMultiCellWidget(titleLineEdit, 0, 0, 1, 4);
    mainLayout->addWidget(artistLabel, 1, 0);
    mainLayout->addMultiCellWidget(artistComboBox, 1, 1, 1, 3);
    mainLayout->addWidget(artistButton, 1, 4);
    mainLayout->addWidget(yearLabel, 2, 0);
    mainLayout->addWidget(yearSpinBox, 2, 1);
    mainLayout->addMultiCellWidget(trackTable, 3, 3, 0, 3);
    mainLayout->addLayout(rightButtonLayout, 3, 4);
    mainLayout->addMultiCellLayout(bottomButtonLayout, 4, 4, 0, 4);
}

void CdForm::accept()
{
    sqlForm->writeFields();
    cdCursor->update();
    db->commit();
    QDialog::accept();
}

void CdForm::reject()
{
    db->rollback();
    QDialog::reject();
}

void CdForm::addNewArtist()
{
    ArtistForm form(this);
    if (form.exec()) {
        artistComboBox->refresh();
        updateButton->setEnabled(artistComboBox->count() > 0);
    }
}

void CdForm::beforeInsertTrack(QSqlRecord *buffer)
{
    buffer->setValue("id", generateId("track", db));
    buffer->setValue("number", trackCursor->size() + 1);
    buffer->setValue("cdid", cdId);
}

void CdForm::beforeDeleteTrack(QSqlRecord *buffer)
{
    QSqlQuery query(db);
    query.exec("UPDATE track SET number = number - 1 "
               "WHERE track.number > "
               + buffer->value("number").toString());
}

void CdForm::moveTrackUp()
{
    QSqlRecord *record = trackTable->currentRecord();
    if (record) {
        int trackNumber = record->value("number").toInt();
        if (trackNumber > 1) {
            swapTracks(trackNumber, trackNumber - 1);
            trackTable->setCurrentCell(trackTable->currentRow() - 1,
                                       trackTable->currentColumn());
        }
    }
}

void CdForm::moveTrackDown()
{
    QSqlRecord *record = trackTable->currentRecord();
    if (record) {
        int trackNumber = record->value("number").toInt();
        if (trackNumber < trackCursor->size()) {
            swapTracks(trackNumber, trackNumber + 1);
            trackTable->setCurrentCell(trackTable->currentRow() + 1,
                                       trackTable->currentColumn());
        }
    }
}

void CdForm::swapTracks(int trackA, int trackB)
{
    QSqlQuery query(db);
    query.exec("UPDATE track SET number = 0 WHERE number = "
               + QString::number(trackA));
    query.exec("UPDATE track SET number = "
               + QString::number(trackA) + " WHERE number = "
               + QString::number(trackB));
    query.exec("UPDATE track SET number = "
               + QString::number(trackB) + " WHERE number = 0");
    trackTable->refresh();
}

void CdForm::createNewRecord()
{
    QSqlQuery query(db);
    int firstArtistId;

    for (;;) {
        query.exec("SELECT id FROM artist");
        if (query.next()) {
            firstArtistId = query.value(0).toInt();
            break;
        }

        ArtistForm form(this);
        form.exec();
    }

    cdId = generateId("cd", db);
    query.exec("INSERT INTO cd (id, artistid, title, year) "
               "VALUES (" + QString::number(cdId)
               + ", " + QString::number(firstArtistId) + "'', "
               + QString::number(QDate::currentDate().year()) + ")");
}
