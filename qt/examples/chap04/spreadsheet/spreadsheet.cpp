#include <qapplication.h>
#include <qclipboard.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qvariant.h>

#include <algorithm>
#include <vector>
using namespace std;

#include "cell.h"
#include "spreadsheet.h"

Spreadsheet::Spreadsheet(QWidget *parent, const char *name)
    : QTable(parent, name)
{
    autoRecalc = true;
    setSelectionMode(Single);
    clear();
}

void Spreadsheet::clear()
{
    setNumRows(0);
    setNumCols(0);
    setNumRows(NumRows);
    setNumCols(NumCols);
    for (int i = 0; i < NumCols; i++)
        horizontalHeader()->setLabel(i, QChar('A' + i));
    setCurrentCell(0, 0);
}

Cell *Spreadsheet::cell(int row, int col) const
{
    return (Cell *)item(row, col);
}

QString Spreadsheet::formula(int row, int col) const
{
    Cell *c = cell(row, col);
    if (c)
        return c->formula();
    else
        return "";
}

void Spreadsheet::setFormula(int row, int col,
                             const QString &formula)
{
    Cell *c = cell(row, col);
    if (c) {
        c->setFormula(formula);
        updateCell(row, col);
    } else {
        setItem(row, col, new Cell(this, formula));
    }
}

QString Spreadsheet::currentLocation() const
{
    return QChar('A' + currentColumn())
           + QString::number(currentRow() + 1);
}

QString Spreadsheet::currentFormula() const
{
    return formula(currentRow(), currentColumn());
}

QWidget *Spreadsheet::createEditor(int row, int col,
                                   bool initFromCell) const
{
    QLineEdit *lineEdit = new QLineEdit(viewport());
    lineEdit->setFrame(false);
    if (initFromCell)
        lineEdit->setText(formula(row, col));
    return lineEdit;
}

void Spreadsheet::endEdit(int row, int col, bool accepted,
                          bool wasReplacing)
{
    QLineEdit *lineEdit = (QLineEdit *)cellWidget(row, col);
    if (!lineEdit)
        return;
    QString oldFormula = formula(row, col);
    QString newFormula = lineEdit->text();

    QTable::endEdit(row, col, false, wasReplacing);

    if (accepted && newFormula != oldFormula) {
        setFormula(row, col, newFormula);
        somethingChanged();
    }
}

void Spreadsheet::somethingChanged()
{
    if (autoRecalc)
        recalculate();
    emit modified();
}

bool Spreadsheet::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.name())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream out(&file);
    out.setVersion(5);

    out << (Q_UINT32)MagicNumber;

    QApplication::setOverrideCursor(waitCursor);
    for (int row = 0; row < NumRows; ++row) {
        for (int col = 0; col < NumCols; ++col) {
            QString str = formula(row, col);
            if (!str.isEmpty())
                out << (Q_UINT16)row << (Q_UINT16)col << str;
        }
    }
    QApplication::restoreOverrideCursor();
    return true;
}

bool Spreadsheet::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(IO_ReadOnly)) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.name())
                             .arg(file.errorString()));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(5);

    Q_UINT32 magic;
    in >> magic;
    if (magic != MagicNumber) {
        QMessageBox::warning(this, tr("Spreadsheet"),
                             tr("The file is not a "
                                "Spreadsheet file."));
        return false;
    }

    clear();

    Q_UINT16 row;
    Q_UINT16 col;
    QString str;

    QApplication::setOverrideCursor(waitCursor);
    while (!in.atEnd()) {
        in >> row >> col >> str;
        setFormula(row, col, str);
    }
    QApplication::restoreOverrideCursor();
    return true;
}

void Spreadsheet::cut()
{
    copy();
    del();
}

void Spreadsheet::copy()
{
    QTableSelection sel = selection();
    QString str;

    for (int i = 0; i < sel.numRows(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < sel.numCols(); ++j) {
            if (j > 0)
                str += "\t";
            str += formula(sel.topRow() + i, sel.leftCol() + j);
        }
    }
    QApplication::clipboard()->setText(str);
}

QTableSelection Spreadsheet::selection()
{
    if (QTable::selection(0).isEmpty())
        return QTableSelection(currentRow(), currentColumn(),
                               currentRow(), currentColumn());
    return QTable::selection(0);
}

void Spreadsheet::paste()
{
    QTableSelection sel = selection();
    QString str = QApplication::clipboard()->text();
    QStringList rows = QStringList::split("\n", str, true);
    int numRows = rows.size();
    int numCols = rows.first().contains("\t") + 1;

    if (sel.numRows() * sel.numCols() != 1
        && (sel.numRows() != numRows
            || sel.numCols() != numCols)) {
        QMessageBox::information(this, tr("Spreadsheet"),
                tr("The information cannot be pasted because the "
                   "copy and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList cols = QStringList::split("\t", rows[i], true);
        for (int j = 0; j < numCols; ++j) {
            int row = sel.topRow() + i;
            int col = sel.leftCol() + j;
            if (row < NumRows && col < NumCols)
                setFormula(row, col, cols[j]);
        }
    }
    somethingChanged();
}

void Spreadsheet::del()
{
    QTableSelection sel = selection();
    for (int i = 0; i < sel.numRows(); ++i) {
        for (int j = 0; j < sel.numCols(); ++j)
            delete cell(sel.topRow() + i, sel.leftCol() + j);
    }
    clearSelection();
}

void Spreadsheet::selectRow()
{
    clearSelection();
    QTable::selectRow(currentRow());
}

void Spreadsheet::selectColumn()
{
    clearSelection();
    QTable::selectColumn(currentColumn());
}

void Spreadsheet::selectAll()
{
    clearSelection();
    selectCells(0, 0, NumRows - 1, NumCols - 1);
}

void Spreadsheet::findNext(const QString &str, bool caseSensitive)
{
    int row = currentRow();
    int col = currentColumn() + 1;

    while (row < NumRows) {
        while (col < NumCols) {
            if (text(row, col).contains(str, caseSensitive)) {
                clearSelection();
                setCurrentCell(row, col);
                setActiveWindow();
                return;
            }
            ++col;
        }
        col = 0;
        ++row;
    }
    qApp->beep();
}

void Spreadsheet::findPrev(const QString &str, bool caseSensitive)
{
    int row = currentRow();
    int col = currentColumn() - 1;

    while (row >= 0) {
        while (col >= 0) {
            if (text(row, col).contains(str, caseSensitive)) {
                clearSelection();
                setCurrentCell(row, col);
                setActiveWindow();
                return;
            }
            --col;
        }
        col = NumCols - 1;
        --row;
    }
    qApp->beep();
}

void Spreadsheet::recalculate()
{
    int row;

    for (row = 0; row < NumRows; ++row) {
        for (int col = 0; col < NumCols; ++col) {
            if (cell(row, col))
                cell(row, col)->setDirty();
        }
    }
    for (row = 0; row < NumRows; ++row) {
        for (int col = 0; col < NumCols; ++col) {
            if (cell(row, col))
                updateCell(row, col);
        }
    }
}

void Spreadsheet::sort(const SpreadsheetCompare &compare)
{
    vector<QStringList> rows;
    QTableSelection sel = selection();
    int i;

    for (i = 0; i < sel.numRows(); ++i) {
        QStringList row;
        for (int j = 0; j < sel.numCols(); ++j)
            row.push_back(formula(sel.topRow() + i,
                                  sel.leftCol() + j));
        rows.push_back(row);
    }

    stable_sort(rows.begin(), rows.end(), compare);

    for (i = 0; i < sel.numRows(); ++i) {
        for (int j = 0; j < sel.numCols(); ++j)
            setFormula(sel.topRow() + i, sel.leftCol() + j,
                       rows[i][j]);
    }

    clearSelection();
    somethingChanged();
}

void Spreadsheet::setAutoRecalculate(bool on)
{
    autoRecalc = on;
    if (autoRecalc)
        recalculate();
}

bool SpreadsheetCompare::operator()(const QStringList &row1,
                                    const QStringList &row2) const
{
    for (int i = 0; i < NumKeys; ++i) {
        int column = keys[i];
        if (column != -1) {
            if (row1[column] != row2[column]) {
                if (ascending[i])
                    return row1[column] < row2[column];
                else
                    return row1[column] > row2[column];
            }
        }
    }
    return false;
}
