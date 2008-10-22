#include <qregexp.h>
#include <qstylesheet.h>
#include <qtextcodec.h>

#include "celldrag.h"

CellDrag::CellDrag(const QString &text, QWidget *parent,
                   const char *name)
    : QDragObject(parent, name)
{
    plainText = text;
}

const char *CellDrag::format(int index) const
{
    switch (index) {
    case 0:
        return "text/csv";
    case 1:
        return "text/html";
    case 2:
        return "text/plain";
    default:
        return 0;
    }
}

QByteArray CellDrag::encodedData(const char *format) const
{
    QByteArray data;
    QTextOStream out(data);

    if (qstrcmp(format, "text/csv") == 0) {
        out << toCsv();
    } else if (qstrcmp(format, "text/html") == 0) {
        out << toHtml();
    } else if (qstrcmp(format, "text/plain") == 0) {
        out << plainText;
    }
    return data;
}

QString CellDrag::toCsv() const
{
    QString out = plainText;
    out.replace("\\", "\\\\");
    out.replace("\"", "\\\"");
    out.replace("\t", "\", \"");
    out.replace("\n", "\"\n\"");
    out.prepend("\"");
    out.append("\"");
    return out;
}

QString CellDrag::toHtml() const
{
    QString out = QStyleSheet::escape(plainText);
    out.replace("\t", "<td>");
    out.replace("\n", "\n<tr><td>");
    out.prepend("<table>\n<tr><td>");
    out.append("\n</table>");
    return out;
}

bool CellDrag::canDecode(const QMimeSource *source)
{
    return source->provides("text/plain");
}

bool CellDrag::decode(const QMimeSource *source, QString &str)
{
    QByteArray data = source->encodedData("text/plain");
    str = QString::fromLocal8Bit((const char *)data, data.size());
    return !str.isEmpty();
}
