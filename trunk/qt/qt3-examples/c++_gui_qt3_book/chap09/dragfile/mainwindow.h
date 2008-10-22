#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

class QTextEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0, const char *name = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    bool readFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QTextEdit *textEdit;
};

#endif
