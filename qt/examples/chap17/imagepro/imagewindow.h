#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <qimage.h>
#include <qmainwindow.h>

#include "transactionthread.h"

class QAction;
class QLabel;

class ImageWindow : public QMainWindow
{
    Q_OBJECT
public:
    ImageWindow(QWidget *parent = 0, const char *name = 0);

protected:
    void closeEvent(QCloseEvent *event);
    void customEvent(QCustomEvent *event);

private slots:
    void open();
    bool save();
    bool saveAs();
    void flipHorizontally();
    void flipVertically();
    void resizeImage();
    void convertTo32Bit();
    void convertTo8Bit();
    void convertTo1Bit();
    void about();

private:
    void createActions();
    void createMenus();
    void createStatusBar();
    bool maybeSave();
    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void addTransaction(Transaction *transact);

    TransactionThread thread;
    QLabel *imageLabel;
    QLabel *infoLabel;
    QLabel *modLabel;
    QCString imageFormat;
    QString curFile;
    bool modified;

    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *convertToSubMenu;
    QPopupMenu *helpMenu;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *flipHorizontallyAct;
    QAction *flipVerticallyAct;
    QAction *rotateAct;
    QAction *resizeAct;
    QAction *convertTo32BitAct;
    QAction *convertTo8BitAct;
    QAction *convertTo1BitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
