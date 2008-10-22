#ifndef SAXHANDLER_H
#define SAXHANDLER_H

#include <qxml.h>

class QListView;
class QListViewItem;

class SaxHandler : public QXmlDefaultHandler
{
public:
    SaxHandler(QListView *view);

    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &attribs);
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    bool fatalError(const QXmlParseException &exception);

private:
    QListView *listView;
    QListViewItem *currentItem;
    QString currentText;
};

#endif
