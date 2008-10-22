#include <qlineedit.h>
#include <qregexp.h>

#include "cell.h"

Cell::Cell(QTable *table, const QString &formula)
    : QTableItem(table, OnTyping)
{
    setFormula(formula);
}

void Cell::setFormula(const QString &formula)
{
    formulaStr = formula;
    cacheIsDirty = true;
}

QString Cell::formula() const
{
    return formulaStr;
}

void Cell::setDirty()
{
    cacheIsDirty = true;
}

QString Cell::text() const
{
    if (value().isValid())
        return value().toString();
    else
        return "####";
}

int Cell::alignment() const
{
    if (value().type() == QVariant::String)
        return AlignLeft | AlignVCenter;
    else
        return AlignRight | AlignVCenter;
}

const QVariant Invalid;

QVariant Cell::value() const
{
    if (cacheIsDirty) {
        cacheIsDirty = false;

        if (formulaStr.startsWith("'")) {
            cachedValue = formulaStr.mid(1);
        } else if (formulaStr.startsWith("=")) {
            cachedValue = Invalid;
            QString expr = formulaStr.mid(1);
            expr.replace(" ", "");
            int pos = 0;
            cachedValue = evalExpression(expr, pos);
            if (pos < (int)expr.length())
                cachedValue = Invalid;
        } else {
            bool ok;
            double d = formulaStr.toDouble(&ok);
            if (ok)
                cachedValue = d;
            else
                cachedValue = formulaStr;
        }
    }
    return cachedValue;
}

QVariant Cell::evalExpression(const QString &str, int &pos) const
{
    QVariant result = evalTerm(str, pos);
    while (pos < (int)str.length()) {
        QChar op = str[pos];
        if (op != '+' && op != '-')
            return result;
        ++pos;

        QVariant term = evalTerm(str, pos);
        if (result.type() == QVariant::Double
                && term.type() == QVariant::Double) {
            if (op == '+')
                result = result.toDouble() + term.toDouble();
            else
                result = result.toDouble() - term.toDouble();
        } else {
            result = Invalid;
        }
    }
    return result;
}

QVariant Cell::evalTerm(const QString &str, int &pos) const
{
    QVariant result = evalFactor(str, pos);
    while (pos < (int)str.length()) {
        QChar op = str[pos];
        if (op != '*' && op != '/')
            return result;
        ++pos;

        QVariant factor = evalFactor(str, pos);
        if (result.type() == QVariant::Double
                && factor.type() == QVariant::Double) {
            if (op == '*') {
                result = result.toDouble() * factor.toDouble();
            } else {
                if (factor.toDouble() == 0.0)
                    result = Invalid;
                else
                    result = result.toDouble() / factor.toDouble();
            }
        } else {
            result = Invalid;
        }
    }
    return result;
}

QVariant Cell::evalFactor(const QString &str, int &pos) const
{
    QVariant result;
    bool negative = false;

    if (str[pos] == '-') {
        negative = true;
        ++pos;
    }

    if (str[pos] == '(') {
        ++pos;
        result = evalExpression(str, pos);
        if (str[pos] != ')')
            result = Invalid;
        ++pos;
    } else {
        QRegExp regExp("[A-Za-z][1-9][0-9]{0,2}");
        QString token;

        while (str[pos].isLetterOrNumber() || str[pos] == '.') {
            token += str[pos];
            ++pos;
        }

        if (regExp.exactMatch(token)) {
            int col = token[0].upper().unicode() - 'A';
            int row = token.mid(1).toInt() - 1;

            Cell *c = (Cell *)table()->item(row, col);
            if (c)
                result = c->value();
            else
                result = 0.0;
        } else {
            bool ok;
            result = token.toDouble(&ok);
            if (!ok)
                result = Invalid;
        }
    }

    if (negative) {
        if (result.type() == QVariant::Double)
            result = -result.toDouble();
        else
            result = Invalid;
    }
    return result;
}
