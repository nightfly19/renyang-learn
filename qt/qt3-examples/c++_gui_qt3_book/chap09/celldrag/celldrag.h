#ifndef CELLDRAG_H
#define CELLDRAG_H

#include <qdragobject.h>

class CellDrag : public QDragObject
{
public:
    CellDrag(const QString &text, QWidget *parent = 0,
             const char *name = 0);

    const char *format(int index) const;
    QByteArray encodedData(const char *format) const;

    static bool canDecode(const QMimeSource *source);
    static bool decode(const QMimeSource *source, QString &str);

private:
    QString toCsv() const;
    QString toHtml() const;

    QString plainText;
};

#endif
