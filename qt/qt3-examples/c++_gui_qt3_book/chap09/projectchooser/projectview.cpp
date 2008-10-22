#include <qapplication.h>
#include <qdragobject.h>

#include "projectview.h"

ProjectView::ProjectView(QWidget *parent, const char *name)
    : QListBox(parent, name)
{
    viewport()->setAcceptDrops(true);
}

void ProjectView::contentsMousePressEvent(QMouseEvent *event)
{
    if (event->button() == LeftButton)
        dragPos = event->pos();
    QListBox::contentsMousePressEvent(event);
}

void ProjectView::contentsMouseMoveEvent(QMouseEvent *event)
{
    if (event->state() & LeftButton) {
        int distance = (event->pos() - dragPos).manhattanLength();
        if (distance > QApplication::startDragDistance())
            startDrag();
    }
    QListBox::contentsMouseMoveEvent(event);
}

void ProjectView::startDrag()
{
    QString person = currentText();
    if (!person.isEmpty()) {
        QTextDrag *drag = new QTextDrag(person, this);
        drag->setSubtype("x-person");
        drag->setPixmap(QPixmap::fromMimeSource("person.png"));
        drag->drag();
    }
}

void ProjectView::contentsDragEnterEvent(QDragEnterEvent *event)
{
    event->accept(event->provides("text/x-person"));
}

void ProjectView::contentsDropEvent(QDropEvent *event)
{
    QString person;

    if (QTextDrag::decode(event, person)) {
        QWidget *fromWidget = event->source();
        if (fromWidget && fromWidget != this
                && fromWidget->inherits("ProjectView")) {
            ProjectView *fromProject = (ProjectView *)fromWidget;
            QListBoxItem *item =
                    fromProject->findItem(person, ExactMatch);
            delete item;
            insertItem(person);
        }
    }
}
