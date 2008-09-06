#include <qdragobject.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qtable.h>
#include <qtextedit.h>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{
    setCaption(tr("Drag File"));
    textEdit = new QTextEdit(this);
    setCentralWidget(textEdit);
    textEdit->viewport()->setAcceptDrops(false);
    setAcceptDrops(true);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept(QUriDrag::canDecode(event));
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QStringList fileNames;
    if (QUriDrag::decodeLocalFiles(event, fileNames)) {
        if (readFile(fileNames[0]))
            setCaption(tr("%1 - Drag File")
                       .arg(strippedName(fileNames[0])));
    }
}

bool MainWindow::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(IO_ReadOnly)) {
        QMessageBox::warning(this, tr("Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(file.name())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    textEdit->setText(in.read());
    return true;
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
