#include <qapplication.h>
#include <qclipboard.h>
#include <qmessagebox.h>

#include "celldrag.h"
#include "mytable.h"

MyTable::MyTable(QWidget *parent, const char *name)
    : QTable(parent, name)
{
    setDragEnabled(true);
    clear();
}

void MyTable::clear()
{
    setNumRows(0);
    setNumCols(0);
    setNumRows(NumRows);
    setNumCols(NumCols);
    for (int i = 0; i < NumCols; i++)
        horizontalHeader()->setLabel(i, QChar('A' + i));
    setCurrentCell(0, 0);
}

QTableSelection MyTable::selection()
{
    if (QTable::selection(0).isEmpty())
        return QTableSelection(currentRow(), currentColumn(),
                               currentRow(), currentColumn());
    return QTable::selection(0);
}

QString MyTable::selectionAsString()
{
    QTableSelection sel = QTable::selection(0);
    QString str;

    for (int i = 0; i < sel.numRows(); ++i) {
        if (i > 0)
            str += "\n";
        for (int j = 0; j < sel.numCols(); ++j) {
            if (j > 0)
                str += "\t";
            str += text(sel.topRow() + i, sel.leftCol() + j);
        }
    }
    return str;
}

void MyTable::copy()
{
    QApplication::clipboard()->setData(dragObject());
}

void MyTable::paste()
{
    QMimeSource *source = QApplication::clipboard()->data();
    if (CellDrag::canDecode(source)) {
        QString str;
        CellDrag::decode(source, str);
        performPaste(str);
    }
}

void MyTable::performPaste(const QString &str)
{
    QTableSelection sel = selection();
    QStringList rows = QStringList::split("\n", str, true);
    int numRows = rows.size();
    int numCols = rows.first().contains("\t") + 1;

    if (sel.numRows() * sel.numCols() != 1
        && (sel.numRows() != numRows
            || sel.numCols() != numCols)) {
        QMessageBox::information(this, tr("My Table"),
                tr("The information cannot be pasted because the "
                   "copy and paste areas aren't the same size."));
        return;
    }

    for (int i = 0; i < numRows; ++i) {
        QStringList cols = QStringList::split("\t", rows[i], true);
        for (int j = 0; j < numCols; ++j)
            setText(sel.topRow() + i, sel.leftCol() + j, cols[j]);
    }
}

QDragObject *MyTable::dragObject()
{
    return new CellDrag(selectionAsString(), this);
}
