#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <qstringlist.h>
#include <qtable.h>

class Cell;
class SpreadsheetCompare;

class Spreadsheet : public QTable
{
    Q_OBJECT
public:
    Spreadsheet(QWidget *parent = 0, const char *name = 0);

    void clear();
    QString currentLocation() const;
    QString currentFormula() const;
    bool autoRecalculate() const { return autoRecalc; }
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);
    QTableSelection selection();
    void sort(const SpreadsheetCompare &compare);

public slots:
    void cut();
    void copy();
    void paste();
    void del();
    void selectRow();
    void selectColumn();
    void selectAll();
    void recalculate();
    void setAutoRecalculate(bool on);
    void findNext(const QString &str, bool caseSensitive);
    void findPrev(const QString &str, bool caseSensitive);

signals:
    void modified();

protected:
    QWidget *createEditor(int row, int col, bool initFromCell) const;
    void endEdit(int row, int col, bool accepted, bool wasReplacing);

private:
    enum { MagicNumber = 0x7F51C882, NumRows = 999, NumCols = 26 };

    Cell *cell(int row, int col) const;
    void setFormula(int row, int col, const QString &formula);
    QString formula(int row, int col) const;
    void somethingChanged();

    bool autoRecalc;
};

class SpreadsheetCompare
{
public:
    bool operator()(const QStringList &row1,
                    const QStringList &row2) const;

    enum { NumKeys = 3 };
    int keys[NumKeys];
    bool ascending[NumKeys];
};

#endif
