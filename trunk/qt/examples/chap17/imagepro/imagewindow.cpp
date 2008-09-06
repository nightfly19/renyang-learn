#include <qaction.h>
#include <qapplication.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qspinbox.h>
#include <qstatusbar.h>

#include "imagewindow.h"
#include "resizedialog.h"

ImageWindow::ImageWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{
    thread.setTargetWidget(this);

    imageLabel = new QLabel(this);
    imageLabel->setSizePolicy(QSizePolicy::Ignored,
                              QSizePolicy::Ignored);
    imageLabel->setBackgroundMode(PaletteDark);
    imageLabel->setAlignment(AlignLeft | AlignTop);
    setCentralWidget(imageLabel);

    createActions();
    createMenus();
    createStatusBar();

    setCurrentFile("");

    modified = false;
}

void ImageWindow::customEvent(QCustomEvent *event)
{
    if ((int)event->type() == TransactionStart) {
        TransactionStartEvent *startEvent =
                (TransactionStartEvent *)event;
        infoLabel->setText(startEvent->message);
    } else if ((int)event->type() == AllTransactionsDone) {
        openAct->setEnabled(true);
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
        imageLabel->setPixmap(QPixmap(thread.image()));
        infoLabel->setText(tr("Ready"));
        modLabel->setText(tr("MOD"));
        modified = true;
        statusBar()->message(tr("Done"), 2000);
    } else {
        QMainWindow::customEvent(event);
    }
}

void ImageWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

void ImageWindow::open()
{
    if (maybeSave()) {
        QString fileName =
                QFileDialog::getOpenFileName(".", "", this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool ImageWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        saveFile(curFile);
        return true;
    }
}

bool ImageWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(".", "", this);
    if (QFile::exists(fileName)) {
        int ret = QMessageBox::warning(this, tr("Image Pro"),
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

void ImageWindow::flipHorizontally()
{
    addTransaction(new FlipTransaction(Horizontal));
}

void ImageWindow::flipVertically()
{
    addTransaction(new FlipTransaction(Vertical));
}

void ImageWindow::resizeImage()
{
    ResizeDialog dialog;
    dialog.widthSpinBox->setValue(imageLabel->pixmap()->width());
    dialog.heightSpinBox->setValue(imageLabel->pixmap()->height());

    if (dialog.exec()) {
        QSize newSize(dialog.widthSpinBox->value(),
                      dialog.heightSpinBox->value());
        addTransaction(new ResizeTransaction(newSize));
    }
}

void ImageWindow::convertTo32Bit()
{
    addTransaction(new ConvertDepthTransaction(32));
}

void ImageWindow::convertTo8Bit()
{
    addTransaction(new ConvertDepthTransaction(8));
}

void ImageWindow::convertTo1Bit()
{
    addTransaction(new ConvertDepthTransaction(1));
}

void ImageWindow::addTransaction(Transaction *transact)
{
    thread.addTransaction(transact);
    openAct->setEnabled(false);
    saveAct->setEnabled(false);
    saveAsAct->setEnabled(false);
}

void ImageWindow::about()
{
    QMessageBox::about(this, tr("About Image Pro"),
            tr("<h2>Image Pro 1.0</h2>"
               "<p>Copyright &copy; 2003 Software Inc."
               "<p>Image Pro is a small application that "
               "demonstrates <b>QCustomEvent</b>, <b>QMutex</b>, "
               "and <b>QThread</b>."));
}

void ImageWindow::createActions()
{
    openAct = new QAction(tr("&Open..."), tr("Ctrl+O"), this);
    openAct->setStatusTip(tr("Open an existing image file"));
    connect(openAct, SIGNAL(activated()),
            this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), tr("Ctrl+S"), this);
    saveAct->setStatusTip(tr("Save the image to disk"));
    connect(saveAct, SIGNAL(activated()),
            this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), 0, this);
    saveAsAct->setStatusTip(tr("Save the image under a new name"));
    connect(saveAsAct, SIGNAL(activated()),
            this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), tr("Ctrl+Q"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(activated()),
            this, SLOT(close()));

    flipHorizontallyAct = new QAction(tr("Flip &Horizontally"),
                                      tr("Ctrl+H"), this);
    flipHorizontallyAct->setStatusTip(tr("Flip the image "
                                         "horizontally"));
    connect(flipHorizontallyAct, SIGNAL(activated()),
            this, SLOT(flipHorizontally()));

    flipVerticallyAct = new QAction(tr("Flip &Vertically"),
                                    tr("Ctrl+V"), this);
    flipVerticallyAct->setStatusTip(tr("Flip the image vertically"));
    connect(flipVerticallyAct, SIGNAL(activated()),
            this, SLOT(flipVertically()));

    resizeAct = new QAction(tr("&Resize..."), tr("Ctrl+R"), this);
    resizeAct->setStatusTip(tr("Resize the image"));
    connect(resizeAct, SIGNAL(activated()),
            this, SLOT(resizeImage()));

    convertTo32BitAct = new QAction(tr("32 Bit"), 0, this);
    convertTo32BitAct->setStatusTip(tr("Convert to 32-bit image"));
    connect(convertTo32BitAct, SIGNAL(activated()),
            this, SLOT(convertTo32Bit()));

    convertTo8BitAct = new QAction(tr("8 Bit"), 0, this);
    convertTo8BitAct->setStatusTip(tr("Convert to 8-bit image"));
    connect(convertTo8BitAct, SIGNAL(activated()),
            this, SLOT(convertTo8Bit()));

    convertTo1BitAct = new QAction(tr("1 Bit"), 0, this);
    convertTo1BitAct->setStatusTip(tr("Convert to 1-bit image"));
    connect(convertTo1BitAct, SIGNAL(activated()),
            this, SLOT(convertTo1Bit()));

    aboutAct = new QAction(tr("&About"), 0, this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(activated()),
            this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), 0, this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(activated()),
            qApp, SLOT(aboutQt()));
}

void ImageWindow::createMenus()
{
    fileMenu = new QPopupMenu(this);
    openAct->addTo(fileMenu);
    saveAct->addTo(fileMenu);
    saveAsAct->addTo(fileMenu);
    fileMenu->insertSeparator();
    exitAct->addTo(fileMenu);

    editMenu = new QPopupMenu(this);
    flipHorizontallyAct->addTo(editMenu);
    flipVerticallyAct->addTo(editMenu);
    resizeAct->addTo(editMenu);
    editMenu->insertSeparator();

    convertToSubMenu = new QPopupMenu(this);
    convertTo32BitAct->addTo(convertToSubMenu);
    convertTo8BitAct->addTo(convertToSubMenu);
    convertTo1BitAct->addTo(convertToSubMenu);
    editMenu->insertItem(tr("&Convert to"), convertToSubMenu);

    helpMenu = new QPopupMenu(this);
    aboutAct->addTo(helpMenu);
    aboutQtAct->addTo(helpMenu);

    menuBar()->insertItem(tr("&File"), fileMenu);
    menuBar()->insertItem(tr("&Edit"), editMenu);
    menuBar()->insertSeparator();
    menuBar()->insertItem(tr("&Help"), helpMenu);
}

void ImageWindow::createStatusBar()
{
    infoLabel = new QLabel(this);

    modLabel = new QLabel(tr(" MOD "), this);
    modLabel->setAlignment(AlignHCenter);
    modLabel->setMinimumSize(modLabel->sizeHint());
    modLabel->clear();

    statusBar()->addWidget(infoLabel, 1);
    statusBar()->addWidget(modLabel);

    infoLabel->setText(tr("Ready"));
}

bool ImageWindow::maybeSave()
{
    if (modified) {
        int ret = QMessageBox::warning(this, tr("Image Pro"),
                     tr("The image has been modified.\n"
                        "Do you want to save your changes?"),
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

void ImageWindow::loadFile(const QString &fileName)
{
    QImage newImage;

    QApplication::setOverrideCursor(waitCursor);
    bool loaded = newImage.load(fileName);
    QApplication::restoreOverrideCursor();

    if (loaded) {
        thread.setImage(newImage);
        imageFormat = QImage::imageFormat(fileName);
        imageLabel->setPixmap(QPixmap(newImage));
        setCurrentFile(fileName);
        statusBar()->message(tr("File loaded"), 2000);
    } else {
        QMessageBox::warning(this, tr("Image Pro"),
                             tr("Error when loading image."));
        statusBar()->message(tr("Loading canceled"), 2000);
    }
}

void ImageWindow::saveFile(const QString &fileName)
{
    QImage image = thread.image();

    QApplication::setOverrideCursor(waitCursor);
    bool saved = image.save(fileName, imageFormat);
    QApplication::restoreOverrideCursor();

    if (saved) {
        setCurrentFile(fileName);
        statusBar()->message(tr("File saved"), 2000);
    } else {
        statusBar()->message(tr("Saving canceled"), 2000);
    }
}

void ImageWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    modLabel->clear();
    modified = false;

    bool hasImage = !curFile.isEmpty();

    if (hasImage) {
        setCaption(tr("%1 - %2").arg(strippedName(curFile))
                                .arg(tr("Image Pro")));
    } else {
        setCaption(tr("Image Pro"));
    }

    saveAct->setEnabled(hasImage);
    saveAsAct->setEnabled(hasImage);
    flipHorizontallyAct->setEnabled(hasImage);
    flipVerticallyAct->setEnabled(hasImage);
    resizeAct->setEnabled(hasImage);
    convertTo32BitAct->setEnabled(hasImage);
    convertTo8BitAct->setEnabled(hasImage);
    convertTo1BitAct->setEnabled(hasImage);
}

QString ImageWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
