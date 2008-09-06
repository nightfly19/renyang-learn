#ifndef EDITOR_H
#define EDITOR_H

#include <qtextedit.h>

class Editor : public QTextEdit
{
    Q_OBJECT
public:
    Editor(QWidget *parent = 0, const char *name = 0);

    void newFile();
    bool open();
    bool openFile(const QString &fileName);
    bool save();
    bool saveAs();
    QSize sizeHint() const;

signals:
    void message(const QString &fileName, int delay);

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool maybeSave();
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    bool readFile(const QString &fileName);
    bool writeFile(const QString &fileName);

    QString curFile;
    bool isUntitled;
    QString fileFilters;
};

#endif
