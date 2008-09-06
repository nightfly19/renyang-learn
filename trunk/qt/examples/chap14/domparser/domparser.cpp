#include <qdom.h>
#include <qlistview.h>

#include "domparser.h"

DomParser::DomParser(QIODevice *device, QListView *view)
{
    listView = view;

    QString errorStr;
    int errorLine;
    int errorColumn;
    QDomDocument doc;
    if (!doc.setContent(device, true, &errorStr, &errorLine,
                        &errorColumn)) {
        qWarning("Line %d, column %d: %s", errorLine, errorColumn,
                 errorStr.ascii());
        return;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "bookindex") {
        qWarning("The file is not a bookindex file");
        return;
    }

    QDomNode node = root.firstChild();
    while (!node.isNull()) {
        if (node.toElement().tagName() == "entry")
            parseEntry(node.toElement(), 0);
        node = node.nextSibling();
    }
}

void DomParser::parseEntry(const QDomElement &element,
                           QListViewItem *parent)
{
    QListViewItem *item;
    if (parent) {
        item = new QListViewItem(parent);
    } else {
        item = new QListViewItem(listView);
    }
    item->setOpen(true);
    item->setText(0, element.attribute("term"));

    QDomNode node = element.firstChild();
    while (!node.isNull()) {
        if (node.toElement().tagName() == "entry") {
            parseEntry(node.toElement(), item);
        } else if (node.toElement().tagName() == "page") {
            QDomNode childNode = node.firstChild();
            while (!childNode.isNull()) {
                if (childNode.nodeType() == QDomNode::TextNode) {
                    QString page = childNode.toText().data();
                    QString allPages = item->text(1);
                    if (!allPages.isEmpty())
                        allPages += ", ";
                    allPages += page;
                    item->setText(1, allPages);
                    break;
                }
                childNode = childNode.nextSibling();
            }
        }
        node = node.nextSibling();
    }
}
