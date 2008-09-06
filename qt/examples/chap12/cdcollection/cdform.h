#ifndef CDFORM_H
#define CDFORM_H

#include <qdialog.h>

class QDataTable;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QSqlCursor;
class QSqlDatabase;
class QSqlForm;
class QSqlRecord;
class ArtistComboBox;

class CdForm : public QDialog
{
    Q_OBJECT
public:
    CdForm(QWidget *parent = 0, const char *name = 0);
    CdForm(int id, QWidget *parent = 0, const char *name = 0);
    ~CdForm();

protected slots:
    void accept();
    void reject();

private slots:
    void addNewArtist();
    void moveTrackUp();
    void moveTrackDown();
    void beforeInsertTrack(QSqlRecord *buffer);
    void beforeDeleteTrack(QSqlRecord *buffer);

private:
    void init();
    void createNewRecord();
    void swapTracks(int trackA, int trackB);

    QLabel *titleLabel;
    QLabel *artistLabel;
    QLabel *yearLabel;
    QLineEdit *titleLineEdit;
    ArtistComboBox *artistComboBox;
    QSpinBox *yearSpinBox;
    QPushButton *artistButton;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QPushButton *updateButton;
    QPushButton *cancelButton;

    QSqlDatabase *db;
    QDataTable *trackTable;
    QSqlForm *sqlForm;
    QSqlCursor *cdCursor;
    QSqlCursor *trackCursor;
    int cdId;
    bool newCd;
};

#endif
