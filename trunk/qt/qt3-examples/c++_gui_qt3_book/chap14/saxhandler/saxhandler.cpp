#include <qlistview.h>

#include "saxhandler.h"

SaxHandler::SaxHandler(QListView *view)
{
    listView = view;
    currentItem = 0;
}

bool SaxHandler::startElement(const QString &, const QString &,
                              const QString &qName,
                              const QXmlAttributes &attribs)
{
    if (qName == "entry") {
        if (currentItem) {
            currentItem = new QListViewItem(currentItem);
        } else {
            currentItem = new QListViewItem(listView);
        }
        currentItem->setOpen(true);
        currentItem->setText(0, attribs.value("term"));
    } else if (qName == "page") {
        currentText = "";
    }
    return true;
}

bool SaxHandler::characters(const QString &str)
{
    currentText += str;
    return true;
}

bool SaxHandler::endElement(const QString &, const QString &,
                            const QString &qName)
{
    if (qName == "entry") {
        currentItem = currentItem->parent();
    } else if (qName == "page") {
        if (currentItem) {
            QString allPages = currentItem->text(1);
            if (!allPages.isEmpty())
                allPages += ", ";
            allPages += currentText;
            currentItem->setText(1, allPages);
        }
    }
    return true;
}

bool SaxHandler::fatalError(const QXmlParseException &exception)
{
    qWarning("Line %d, column %d: %s", exception.lineNumber(),
             exception.columnNumber(), exception.message().ascii());
    return false;
}
