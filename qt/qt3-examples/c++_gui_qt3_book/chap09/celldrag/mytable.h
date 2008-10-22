#ifndef MYTABLE_H
#define MYTABLE_H

#include <qtable.h>

class MyTable : public QTable
{
    Q_OBJECT
public:
    MyTable(QWidget *parent = 0, const char *name = 0);

    void clear();
    QTableSelection selection();

public slots:
    void copy();
    void paste();

protected:
    QDragObject *dragObject();

private:
    enum { NumRows = 999, NumCols = 26 };

    QString selectionAsString();
    void performPaste(const QString &str);
};

#endif
