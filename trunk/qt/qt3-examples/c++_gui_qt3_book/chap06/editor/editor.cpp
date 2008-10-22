#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpixmap.h>

#include "editor.h"

Editor::Editor(QWidget *parent, const char *name)
    : QTextEdit(parent, name)
{
    setWFlags(WDestructiveClose);
    setIcon(QPixmap::fromMimeSource("document.png"));

    isUntitled = true;
    fileFilters = tr("Text files (*.txt)\n"
                     "All files (*)");
}

void Editor::newFile()
{
    static int documentNumber = 1;

    curFile = tr("document%1.txt").arg(documentNumber);
    setCaption(curFile);
    isUntitled = true;
    ++documentNumber;
}

bool Editor::open()
{
    QString fileName =
            QFileDialog::getOpenFileName(".", fileFilters, this);
    if (fileName.isEmpty())
        return false;

    return openFile(fileName);
}

bool Editor::openFile(const QString &fileName)
{
    bool ok = readFile(fileName);
    if (ok) {
        setCurrentFile(fileName);
        emit message(tr("File loaded"), 2000);
    } else {
        emit message(tr("Loading canceled"), 2000);
    }
    return ok;
}

bool Editor::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        saveFile(curFile);
        return true;
    }
}

bool Editor::saveAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(curFile, fileFilters, this);
    if (fileName.isEmpty())
        return false;

    if (QFile::exists(fileName)) {
        int ret = QMessageBox::warning(this, tr("Editor"),
                     tr("File %1 already exists.\n"
                        "Do you want to overwrite it?")
                     .arg(QDir::convertSeparators(fileName)),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No | QMessageBox::Escape);
        if (ret == QMessageBox::No)
            return true;
    }
    if (!fileName.isEmpty())
        saveFile(fileName);
    return true;
}

void Editor::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

bool Editor::maybeSave()
{
    if (isModified()) {
        int ret = QMessageBox::warning(this, tr("Editor"),
                     tr("File %1 has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(strippedName(curFile)),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void Editor::saveFile(const QString &fileName)
{
    if (writeFile(fileName)) {
        setCurrentFile(fileName);
        emit message(tr("File saved"), 2000);
    } else {
        emit message(tr("Saving canceled"), 2000);
    }
}

void Editor::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setCaption(strippedName(curFile));
    isUntitled = false;
    setModified(false);
}

QSize Editor::sizeHint() const
{
    return QSize(72 * fontMetrics().width('x'),
                 25 * fontMetrics().lineSpacing());
}

bool Editor::readFile(const QString &fileName)
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
    QApplication::setOverrideCursor(waitCursor);
    setText(in.read());
    QApplication::restoreOverrideCursor();
    return true;
}

bool Editor::writeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(IO_WriteOnly)) {
        QMessageBox::warning(this, tr("Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(file.name())
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(waitCursor);
    out << text();
    QApplication::restoreOverrideCursor();
    return true;
}

QString Editor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
