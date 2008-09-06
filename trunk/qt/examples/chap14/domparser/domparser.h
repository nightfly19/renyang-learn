#ifndef DOMPARSER_H
#define DOMPARSER_H

class QDomElement;
class QListView;
class QListViewItem;
class QIODevice;

class DomParser
{
public:
    DomParser(QIODevice *device, QListView *view);

private:
    void parseEntry(const QDomElement &element,
                    QListViewItem *parent);

    QListView *listView;
};

#endif
