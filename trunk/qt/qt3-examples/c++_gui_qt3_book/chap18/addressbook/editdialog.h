#ifndef EDITDIALOG_H
#define EDITDILAOG_H

#include <qdialog.h>

class QLabel;
class QLineEdit;
class QListView;
class QPushButton;
class ABItem;

class EditDialog : public QDialog
{
    Q_OBJECT
public:
    EditDialog(QListView *view, QWidget *parent = 0,
               const char *name = 0);
    EditDialog(ABItem *item, QWidget *parent = 0,
               const char *name = 0);

protected slots:
    void accept();

private:
    void init();

    QListView *listView;
    ABItem *currentItem;
    QLabel *contactLabel;
    QLabel *addressLabel;
    QLabel *phoneLabel;
    QLineEdit *contactLineEdit;
    QLineEdit *addressLineEdit;
    QLineEdit *phoneLineEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif
