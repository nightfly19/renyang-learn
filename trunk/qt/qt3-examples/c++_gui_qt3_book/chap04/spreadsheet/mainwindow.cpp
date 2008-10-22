#include <qaction.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qfiledialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qsettings.h>
#include <qstatusbar.h>

#include "cell.h"
#include "finddialog.h"
#include "gotocelldialog.h"
#include "mainwindow.h"
#include "sortdialog.h"
#include "spreadsheet.h"

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{
    spreadsheet = new Spreadsheet(this);
    setCentralWidget(spreadsheet);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    setCaption(tr("Spreadsheet"));
    setIcon(QPixmap::fromMimeSource("icon.png"));

    findDialog = 0;
    fileFilters = tr("Spreadsheet files (*.sp)");
    modified = false;
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), tr("Ctrl+N"), this);
    newAct->setIconSet(QPixmap::fromMimeSource("new.png"));
    newAct->setStatusTip(tr("Create a new spreadsheet file"));
    connect(newAct, SIGNAL(activated()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), tr("Ctrl+O"), this);
    openAct->setIconSet(QPixmap::fromMimeSource("open.png"));
    openAct->setStatusTip(tr("Open an existing spreadsheet file"));
    connect(openAct, SIGNAL(activated()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), tr("Ctrl+S"), this);
    saveAct->setIconSet(QPixmap::fromMimeSource("save.png"));
    saveAct->setStatusTip(tr("Save the spreadsheet to disk"));
    connect(saveAct, SIGNAL(activated()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), 0, this);
    saveAsAct->setStatusTip(tr("Save the spreadsheet under a new "
                               "name"));
    connect(saveAsAct, SIGNAL(activated()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), tr("Ctrl+Q"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(activated()), this, SLOT(close()));

    cutAct = new QAction(tr("Cu&t"), tr("Ctrl+X"), this);
    cutAct->setIconSet(QPixmap::fromMimeSource("cut.png"));
    cutAct->setStatusTip(tr("Cut the current selection's contents "
                            "to the clipboard"));
    connect(cutAct, SIGNAL(activated()), spreadsheet, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), tr("Ctrl+C"), this);
    copyAct->setIconSet(QPixmap::fromMimeSource("copy.png"));
    copyAct->setStatusTip(tr("Copy the current selection's contents "
                             "to the clipboard"));
    connect(copyAct, SIGNAL(activated()), spreadsheet, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), tr("Ctrl+V"), this);
    pasteAct->setIconSet(QPixmap::fromMimeSource("paste.png"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into "
                              "the current selection"));
    connect(pasteAct, SIGNAL(activated()),
            spreadsheet, SLOT(paste()));

    deleteAct = new QAction(tr("&Delete"), tr("Del"), this);
    deleteAct->setIconSet(QPixmap::fromMimeSource("delete.png"));
    deleteAct->setStatusTip(tr("Delete the current selection's "
                               "contents"));
    connect(deleteAct, SIGNAL(activated()),
            spreadsheet, SLOT(del()));

    selectRowAct = new QAction(tr("&Row"), 0, this);
    selectRowAct->setStatusTip(tr("Select all the cells in the "
                                  "current row"));
    connect(selectRowAct, SIGNAL(activated()),
            spreadsheet, SLOT(selectRow()));

    selectColumnAct = new QAction(tr("&Column"), 0, this);
    selectColumnAct->setStatusTip(tr("Select all the cells in the "
                                     "current column"));
    connect(selectColumnAct, SIGNAL(activated()),
            spreadsheet, SLOT(selectColumn()));

    selectAllAct = new QAction(tr("&All"), tr("Ctrl+A"), this);
    selectAllAct->setStatusTip(tr("Select all the cells in the "
                                  "spreadsheet"));
    connect(selectAllAct, SIGNAL(activated()),
            spreadsheet, SLOT(selectAll()));

    findAct = new QAction(tr("&Find..."), tr("Ctrl+F"), this);
    findAct->setIconSet(QPixmap::fromMimeSource("find.png"));
    findAct->setStatusTip(tr("Find a matching cell"));
    connect(findAct, SIGNAL(activated()), this, SLOT(find()));

    goToCellAct = new QAction(tr("&Go to Cell..."), tr("F5"), this);
    goToCellAct->setIconSet(QPixmap::fromMimeSource("gotocell.png"));
    goToCellAct->setStatusTip(tr("Go to the specified cell"));
    connect(goToCellAct, SIGNAL(activated()),
            this, SLOT(goToCell()));

    recalculateAct = new QAction(tr("&Recalculate"), tr("F9"), this);
    recalculateAct->setStatusTip(tr("Recalculate all the "
                                    "spreadsheet's formulas"));
    connect(recalculateAct, SIGNAL(activated()),
            spreadsheet, SLOT(recalculate()));

    sortAct = new QAction(tr("&Sort..."), 0, this);
    sortAct->setStatusTip(tr("Sort the selected cells or all the "
                             "cells"));
    connect(sortAct, SIGNAL(activated()), this, SLOT(sort()));

    showGridAct = new QAction(tr("&Show Grid"), 0, this);
    showGridAct->setToggleAction(true);
    showGridAct->setOn(spreadsheet->showGrid());
    showGridAct->setStatusTip(tr("Show or hide the spreadsheet's "
                                 "grid"));
    connect(showGridAct, SIGNAL(toggled(bool)),
            spreadsheet, SLOT(setShowGrid(bool)));

    autoRecalcAct = new QAction(tr("&Auto-recalculate"), 0, this);
    autoRecalcAct->setToggleAction(true);
    autoRecalcAct->setOn(spreadsheet->autoRecalculate());
    autoRecalcAct->setStatusTip(tr("Switch auto-recalculation on or "
                                   "off"));
    connect(autoRecalcAct, SIGNAL(toggled(bool)),
            spreadsheet, SLOT(setAutoRecalculate(bool)));

    aboutAct = new QAction(tr("&About"), 0, this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(activated()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), 0, this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(activated()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = new QPopupMenu(this);
    newAct->addTo(fileMenu);
    openAct->addTo(fileMenu);
    saveAct->addTo(fileMenu);
    saveAsAct->addTo(fileMenu);
    fileMenu->insertSeparator();
    exitAct->addTo(fileMenu);

    for (int i = 0; i < MaxRecentFiles; ++i)
        recentFileIds[i] = -1;

    editMenu = new QPopupMenu(this);
    cutAct->addTo(editMenu);
    copyAct->addTo(editMenu);
    pasteAct->addTo(editMenu);
    deleteAct->addTo(editMenu);

    selectSubMenu = new QPopupMenu(this);
    selectRowAct->addTo(selectSubMenu);
    selectColumnAct->addTo(selectSubMenu);
    selectAllAct->addTo(selectSubMenu);
    editMenu->insertItem(tr("&Select"), selectSubMenu);

    editMenu->insertSeparator();
    findAct->addTo(editMenu);
    goToCellAct->addTo(editMenu);

    toolsMenu = new QPopupMenu(this);
    recalculateAct->addTo(toolsMenu);
    sortAct->addTo(toolsMenu);

    optionsMenu = new QPopupMenu(this);
    showGridAct->addTo(optionsMenu);
    autoRecalcAct->addTo(optionsMenu);

    helpMenu = new QPopupMenu(this);
    aboutAct->addTo(helpMenu);
    aboutQtAct->addTo(helpMenu);

    menuBar()->insertItem(tr("&File"), fileMenu);
    menuBar()->insertItem(tr("&Edit"), editMenu);
    menuBar()->insertItem(tr("&Tools"), toolsMenu);
    menuBar()->insertItem(tr("&Options"), optionsMenu);
    menuBar()->insertSeparator();
    menuBar()->insertItem(tr("&Help"), helpMenu);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QPopupMenu contextMenu(this);
    cutAct->addTo(&contextMenu);
    copyAct->addTo(&contextMenu);
    pasteAct->addTo(&contextMenu);
    contextMenu.exec(event->globalPos());
}

void MainWindow::createToolBars()
{
    fileToolBar = new QToolBar(tr("File"), this);
    newAct->addTo(fileToolBar);
    openAct->addTo(fileToolBar);
    saveAct->addTo(fileToolBar);

    editToolBar = new QToolBar(tr("Edit"), this);
    cutAct->addTo(editToolBar);
    copyAct->addTo(editToolBar);
    pasteAct->addTo(editToolBar);
    editToolBar->addSeparator();
    findAct->addTo(editToolBar);
    goToCellAct->addTo(editToolBar);
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        spreadsheet->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName =
                QFileDialog::getOpenFileName(".", fileFilters, this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    if (spreadsheet->readFile(fileName)) {
        setCurrentFile(fileName);
        statusBar()->message(tr("File loaded"), 2000);
    } else {
        statusBar()->message(tr("Loading canceled"), 2000);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        saveFile(curFile);
        return true;
    }
}

void MainWindow::saveFile(const QString &fileName)
{
    if (spreadsheet->writeFile(fileName)) {
        setCurrentFile(fileName);
        statusBar()->message(tr("File saved"), 2000);
    } else {
        statusBar()->message(tr("Saving canceled"), 2000);
    }
}

bool MainWindow::saveAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(".", fileFilters, this);
    if (fileName.isEmpty())
        return false;

    if (QFile::exists(fileName)) {
        int ret = QMessageBox::warning(this, tr("Spreadsheet"),
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave()
{
    if (modified) {
        int ret = QMessageBox::warning(this, tr("Spreadsheet"),
                     tr("The document has been modified.\n"
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

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    modLabel->clear();
    modified = false;

    if (curFile.isEmpty()) {
        setCaption(tr("Spreadsheet"));
    } else {
        setCaption(tr("%1 - %2").arg(strippedName(curFile))
                                .arg(tr("Spreadsheet")));
        recentFiles.remove(curFile);
        recentFiles.push_front(curFile);
        updateRecentFileItems();
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::updateRecentFileItems()
{
    while ((int)recentFiles.size() > MaxRecentFiles)
        recentFiles.pop_back();

    for (int i = 0; i < (int)recentFiles.size(); ++i) {
        QString text = tr("&%1 %2")
                       .arg(i + 1)
                       .arg(strippedName(recentFiles[i]));
        if (recentFileIds[i] == -1) {
            if (i == 0)
                fileMenu->insertSeparator(fileMenu->count() - 2);
            recentFileIds[i] =
                    fileMenu->insertItem(text, this,
                                         SLOT(openRecentFile(int)),
                                         0, -1,
                                         fileMenu->count() - 2);
            fileMenu->setItemParameter(recentFileIds[i], i);
        } else {
            fileMenu->changeItem(recentFileIds[i], text);
        }
    }
}

void MainWindow::openRecentFile(int param)
{
    if (maybeSave())
        loadFile(recentFiles[param]);
}

void MainWindow::createStatusBar()
{
    locationLabel = new QLabel(" W999 ", this);
    locationLabel->setAlignment(AlignHCenter);
    locationLabel->setMinimumSize(locationLabel->sizeHint());

    formulaLabel = new QLabel(this);

    modLabel = new QLabel(tr(" MOD "), this);
    modLabel->setAlignment(AlignHCenter);
    modLabel->setMinimumSize(modLabel->sizeHint());
    modLabel->clear();

    statusBar()->addWidget(locationLabel);
    statusBar()->addWidget(formulaLabel, 1);
    statusBar()->addWidget(modLabel);

    connect(spreadsheet, SIGNAL(currentChanged(int, int)),
            this, SLOT(updateCellIndicators()));
    connect(spreadsheet, SIGNAL(modified()),
            this, SLOT(spreadsheetModified()));

    updateCellIndicators();
}

void MainWindow::updateCellIndicators()
{
    locationLabel->setText(spreadsheet->currentLocation());
    formulaLabel->setText(" " + spreadsheet->currentFormula());
}

void MainWindow::spreadsheetModified()
{
    modLabel->setText(tr("MOD"));
    modified = true;
    updateCellIndicators();
}

void MainWindow::find()
{
    if (!findDialog) {
        findDialog = new FindDialog(this);
        connect(findDialog, SIGNAL(findNext(const QString &, bool)),
                spreadsheet, SLOT(findNext(const QString &, bool)));
        connect(findDialog, SIGNAL(findPrev(const QString &, bool)),
                spreadsheet, SLOT(findPrev(const QString &, bool)));
    }

    findDialog->show();
    findDialog->raise();
    findDialog->setActiveWindow();
}

void MainWindow::goToCell()
{
    GoToCellDialog dialog(this);
    if (dialog.exec()) {
        QString str = dialog.lineEdit->text();
        spreadsheet->setCurrentCell(str.mid(1).toInt() - 1,
                                    str[0].upper().unicode() - 'A');
    }
}

void MainWindow::sort()
{
    SortDialog dialog(this);
    QTableSelection sel = spreadsheet->selection();
    dialog.setColumnRange('A' + sel.leftCol(), 'A' + sel.rightCol());

    if (dialog.exec()) {
        SpreadsheetCompare compare;
        compare.keys[0] =
              dialog.primaryColumnCombo->currentItem();
        compare.keys[1] =
              dialog.secondaryColumnCombo->currentItem() - 1;
        compare.keys[2] =
              dialog.tertiaryColumnCombo->currentItem() - 1;
        compare.ascending[0] =
              (dialog.primaryOrderCombo->currentItem() == 0);
        compare.ascending[1] =
              (dialog.secondaryOrderCombo->currentItem() == 0);
        compare.ascending[2] =
              (dialog.tertiaryOrderCombo->currentItem() == 0);
        spreadsheet->sort(compare);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Spreadsheet"),
            tr("<h2>Spreadsheet 1.0</h2>"
               "<p>Copyright &copy; 2003 Software Inc."
               "<p>Spreadsheet is a small application that "
               "demonstrates <b>QAction</b>, <b>QMainWindow</b>, "
               "<b>QMenuBar</b>, <b>QStatusBar</b>, "
               "<b>QToolBar</b>, and many other Qt classes."));
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setPath("software-inc.com", "Spreadsheet");
    settings.beginGroup("/Spreadsheet");
    settings.writeEntry("/geometry/x", x());
    settings.writeEntry("/geometry/y", y());
    settings.writeEntry("/geometry/width", width());
    settings.writeEntry("/geometry/height", height());
    settings.writeEntry("/recentFiles", recentFiles);
    settings.writeEntry("/showGrid", showGridAct->isOn());
    settings.writeEntry("/autoRecalc", showGridAct->isOn());
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings;
    settings.setPath("software-inc.com", "Spreadsheet");
    settings.beginGroup("/Spreadsheet");

    int x = settings.readNumEntry("/geometry/x", 200);
    int y = settings.readNumEntry("/geometry/y", 200);
    int w = settings.readNumEntry("/geometry/width", 400);
    int h = settings.readNumEntry("/geometry/height", 400);
    move(x, y);
    resize(w, h);

    recentFiles = settings.readListEntry("/recentFiles");
    updateRecentFileItems();

    showGridAct->setOn(
            settings.readBoolEntry("/showGrid", true));
    autoRecalcAct->setOn(
            settings.readBoolEntry("/autoRecalc", true));

    settings.endGroup();
}
