#include <qapplication.h>
#include <qdatatable.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qsqldatabase.h>
#include <qsqlselectcursor.h>

#include <cstdlib>
using namespace std;

#include "cdform.h"
#include "mainform.h"

MainForm::MainForm(QWidget *parent, const char *name)
    : QDialog(parent, name)
{
    setCaption(tr("CD Collection"));

    splitter = new QSplitter(Vertical, this);

    QSqlSelectCursor *cdCursor = new QSqlSelectCursor(
                    "SELECT cd.id, title, name, country, year "
                    "FROM cd, artist WHERE cd.artistid = artist.id");
    if (!cdCursor->isActive()) {
        QMessageBox::critical(this, tr("CD Collection"),
                tr("The database has not been created.\n"
                "Run the cdtables example to create a sample "
                "database, then copy cdcollection.dat into "
                "this directory and restart this application."));
        qApp->quit();
    }

    cdTable = new QDataTable(cdCursor, false, splitter);
    cdTable->addColumn("title", tr("CD"));
    cdTable->addColumn("name", tr("Artist"));
    cdTable->addColumn("country", tr("Country"));
    cdTable->addColumn("year", tr("Year"));
    cdTable->setAutoDelete(true);
    cdTable->refresh();

    QSqlCursor *trackCursor = new QSqlCursor("track");
    trackCursor->setMode(QSqlCursor::ReadOnly);
    trackTable = new QDataTable(trackCursor, false, splitter);
    trackTable->setSort(trackCursor->index("number"));
    trackTable->addColumn("title", tr("Track"));
    trackTable->addColumn("duration", tr("Duration"));
    trackTable->setAutoDelete(true);
    trackTable->refresh();

    addButton = new QPushButton(tr("&Add"), this);
    editButton = new QPushButton(tr("&Edit"), this);
    deleteButton = new QPushButton(tr("&Delete"), this);
    refreshButton = new QPushButton(tr("&Refresh"), this);
    quitButton = new QPushButton(tr("&Quit"), this);

    connect(addButton, SIGNAL(clicked()),
            this, SLOT(addCd()));
    connect(editButton, SIGNAL(clicked()),
            this, SLOT(editCd()));
    connect(deleteButton, SIGNAL(clicked()),
            this, SLOT(deleteCd()));
    connect(refreshButton, SIGNAL(clicked()),
            cdTable, SLOT(refresh()));
    connect(quitButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(cdTable, SIGNAL(currentChanged(QSqlRecord *)),
            this, SLOT(currentCdChanged(QSqlRecord *)));
    connect(cdTable,
            SIGNAL(doubleClicked(int, int, int, const QPoint &)),
            this, SLOT(editCd()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(quitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addWidget(splitter);
    mainLayout->addLayout(buttonLayout);
}

void MainForm::addCd()
{
    CdForm form(this);
    if (form.exec()) {
        cdTable->refresh();
        trackTable->refresh();
    }
}

void MainForm::editCd()
{
    QSqlRecord *record = cdTable->currentRecord();
    if (record) {
        CdForm form(record->value("id").toInt(), this);
        if (form.exec()) {
            cdTable->refresh();
            trackTable->refresh();
        }
    }
}

void MainForm::deleteCd()
{
    QSqlRecord *record = cdTable->currentRecord();
    if (record) {
        QSqlQuery query;
        query.exec("DELETE FROM track WHERE cdid = "
                   + record->value("id").toString());
        query.exec("DELETE FROM cd WHERE id = "
                   + record->value("id").toString());
        cdTable->refresh();
        trackTable->refresh();
    }
}

void MainForm::currentCdChanged(QSqlRecord *record)
{
    trackTable->setFilter("cdid = "
                          + record->value("id").toString());
    trackTable->refresh();
}
