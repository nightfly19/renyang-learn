#ifndef CELL_H
#define CELL_H

#include <qtable.h>
#include <qvariant.h>

class Cell : public QTableItem
{
public:
    Cell(QTable *table, const QString &formula);

    void setFormula(const QString &formula);
    QString formula() const;
    void setDirty();
    QString text() const;
    int alignment() const;

private:
    QVariant value() const;
    QVariant evalExpression(const QString &str, int &pos) const;
    QVariant evalTerm(const QString &str, int &pos) const;
    QVariant evalFactor(const QString &str, int &pos) const;

    QString formulaStr;
    mutable QVariant cachedValue;
    mutable bool cacheIsDirty;
};

#endif
