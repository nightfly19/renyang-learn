#ifndef MAINFORM_H
#define MAINFORM_H

#include <qdialog.h>

class QDataTable;
class QPushButton;
class QSplitter;
class QSqlRecord;

class MainForm : public QDialog
{
    Q_OBJECT
public:
    MainForm(QWidget *parent = 0, const char *name = 0);

private slots:
    void addCd();
    void editCd();
    void deleteCd();
    void currentCdChanged(QSqlRecord *record);

private:
    QSplitter *splitter;
    QDataTable *cdTable;
    QDataTable *trackTable;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *refreshButton;
    QPushButton *quitButton;
};

#endif
