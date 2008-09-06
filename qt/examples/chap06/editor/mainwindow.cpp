#include <qaction.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qstatusbar.h>
#include <qworkspace.h>

#include "editor.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent, name)
{
    workspace = new QWorkspace(this);
    setCentralWidget(workspace);
    connect(workspace, SIGNAL(windowActivated(QWidget *)),
            this, SLOT(updateMenus()));
    connect(workspace, SIGNAL(windowActivated(QWidget *)),
            this, SLOT(updateModIndicator()));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    setCaption(tr("Editor"));
    setIcon(QPixmap::fromMimeSource("icon.png"));
}

void MainWindow::createActions()
{
    newAct = new QAction(tr("&New"), tr("Ctrl+N"), this);
    newAct->setIconSet(QPixmap::fromMimeSource("new.png"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(activated()), this, SLOT(newFile()));

    openAct = new QAction(tr("&Open..."), tr("Ctrl+O"), this);
    openAct->setIconSet(QPixmap::fromMimeSource("open.png"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(activated()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), tr("Ctrl+S"), this);
    saveAct->setIconSet(QPixmap::fromMimeSource("save.png"));
    saveAct->setStatusTip(tr("Save the file to disk"));
    connect(saveAct, SIGNAL(activated()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), 0, this);
    saveAsAct->setStatusTip(tr("Save the file under a new name"));
    connect(saveAsAct, SIGNAL(activated()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), tr("Ctrl+Q"), this);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(activated()), this, SLOT(close()));

    cutAct = new QAction(tr("Cu&t"), tr("Ctrl+X"), this);
    cutAct->setIconSet(QPixmap::fromMimeSource("cut.png"));
    cutAct->setStatusTip(tr("Cut the current selection to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(activated()), this, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), tr("Ctrl+C"), this);
    copyAct->setIconSet(QPixmap::fromMimeSource("copy.png"));
    copyAct->setStatusTip(tr("Copy the current selection to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(activated()), this, SLOT(copy()));

    pasteAct = new QAction(tr("&Paste"), tr("Ctrl+V"), this);
    pasteAct->setIconSet(QPixmap::fromMimeSource("paste.png"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents at "
                              "the cursor position"));
    connect(pasteAct, SIGNAL(activated()), this, SLOT(paste()));

    deleteAct = new QAction(tr("&Delete"), tr("Del"), this);
    deleteAct->setIconSet(QPixmap::fromMimeSource("delete.png"));
    deleteAct->setStatusTip(tr("Delete the current selection"));
    connect(deleteAct, SIGNAL(activated()), this, SLOT(del()));

    closeAct = new QAction(tr("Cl&ose"), tr("Ctrl+F4"), this);
    closeAct->setStatusTip(tr("Close the active window"));
    connect(closeAct, SIGNAL(activated()),
            workspace, SLOT(closeActiveWindow()));

    closeAllAct = new QAction(tr("Close &All"), 0, this);
    closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(closeAllAct, SIGNAL(activated()),
            workspace, SLOT(closeAllWindows()));

    tileAct = new QAction(tr("&Tile"), 0, this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(activated()), workspace, SLOT(tile()));

    cascadeAct = new QAction(tr("&Cascade"), 0, this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(activated()),
            workspace, SLOT(cascade()));

    nextAct = new QAction(tr("Ne&xt"), tr("Ctrl+F6"), this);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(activated()),
            workspace, SLOT(activateNextWindow()));

    previousAct = new QAction(tr("Pre&vious"),
                              tr("Ctrl+Shift+F6"), this);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(activated()),
            workspace, SLOT(activatePreviousWindow()));

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

    editMenu = new QPopupMenu(this);
    cutAct->addTo(editMenu);
    copyAct->addTo(editMenu);
    pasteAct->addTo(editMenu);
    deleteAct->addTo(editMenu);

    windowsMenu = new QPopupMenu(this);
    createWindowsMenu();

    helpMenu = new QPopupMenu(this);
    aboutAct->addTo(helpMenu);
    aboutQtAct->addTo(helpMenu);

    menuBar()->insertItem(tr("&File"), fileMenu);
    menuBar()->insertItem(tr("&Edit"), editMenu);
    menuBar()->insertItem(tr("&Windows"), windowsMenu);
    menuBar()->insertSeparator();
    menuBar()->insertItem(tr("&Help"), helpMenu);
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
}

void MainWindow::createStatusBar()
{
    readyLabel = new QLabel(tr(" Ready"), this);

    modLabel = new QLabel(tr(" MOD "), this);
    modLabel->setAlignment(AlignHCenter);
    modLabel->setMinimumSize(modLabel->sizeHint());
    modLabel->clear();

    statusBar()->addWidget(readyLabel, 1);
    statusBar()->addWidget(modLabel);
}

void MainWindow::newFile()
{
    Editor *editor = createEditor();
    editor->newFile();
    editor->show();
}

Editor *MainWindow::createEditor()
{
    Editor *editor = new Editor(workspace);
    connect(editor, SIGNAL(copyAvailable(bool)),
            this, SLOT(copyAvailable(bool)));
    connect(editor, SIGNAL(modificationChanged(bool)),
            this, SLOT(updateModIndicator()));
    return editor;
}

void MainWindow::open()
{
    Editor *editor = createEditor();
    if (editor->open())
        editor->show();
    else
        editor->close();
}

void MainWindow::openFile(const QString &fileName)
{
    Editor *editor = createEditor();
    if (editor->openFile(fileName))
        editor->show();
    else
        editor->close();
}

void MainWindow::save()
{
    if (activeEditor()) {
        activeEditor()->save();
        updateModIndicator();
    }
}

Editor *MainWindow::activeEditor()
{
    return (Editor *)workspace->activeWindow();
}

void MainWindow::saveAs()
{
    if (activeEditor()) {
        activeEditor()->saveAs();
        updateModIndicator();
    }
}

void MainWindow::cut()
{
    if (activeEditor())
        activeEditor()->cut();
}

void MainWindow::copy()
{
    if (activeEditor())
        activeEditor()->copy();
}

void MainWindow::paste()
{
    if (activeEditor())
        activeEditor()->paste();
}

void MainWindow::del()
{
    if (activeEditor())
        activeEditor()->del();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Editor"),
            tr("<h2>Editor 1.0</h2>"
               "<p>Copyright &copy; 2004 Software Inc."
               "<p>Editor is a small application that demonstrates "
               "<b>QWorkspace</b>."));
}

void MainWindow::updateMenus()
{
    bool hasEditor = (activeEditor() != 0);
    saveAct->setEnabled(hasEditor);
    saveAsAct->setEnabled(hasEditor);
    pasteAct->setEnabled(hasEditor);
    deleteAct->setEnabled(hasEditor);
    copyAvailable(activeEditor()
                  && activeEditor()->hasSelectedText());
    closeAct->setEnabled(hasEditor);
    closeAllAct->setEnabled(hasEditor);
    tileAct->setEnabled(hasEditor);
    cascadeAct->setEnabled(hasEditor);
    nextAct->setEnabled(hasEditor);
    previousAct->setEnabled(hasEditor);

    windowsMenu->clear();
    createWindowsMenu();
}

void MainWindow::createWindowsMenu()
{
    closeAct->addTo(windowsMenu);
    closeAllAct->addTo(windowsMenu);
    windowsMenu->insertSeparator();
    tileAct->addTo(windowsMenu);
    cascadeAct->addTo(windowsMenu);
    windowsMenu->insertSeparator();
    nextAct->addTo(windowsMenu);
    previousAct->addTo(windowsMenu);

    if (activeEditor()) {
        windowsMenu->insertSeparator();
        windows = workspace->windowList();
        int numVisibleEditors = 0;

        for (int i = 0; i < (int)windows.count(); ++i) {
            QWidget *win = windows.at(i);
            if (!win->isHidden()) {
                QString text = tr("%1 %2")
                               .arg(numVisibleEditors + 1)
                               .arg(win->caption());
                if (numVisibleEditors < 9)
                    text.prepend("&");
                int id = windowsMenu->insertItem(
                              text, this, SLOT(activateWindow(int)));
                bool isActive = (activeEditor() == win);
                windowsMenu->setItemChecked(id, isActive);
                windowsMenu->setItemParameter(id, i);
                ++numVisibleEditors;
            }
        }
    }
}

void MainWindow::activateWindow(int param)
{
    QWidget *win = windows.at(param);
    win->show();
    win->setFocus();
}

void MainWindow::copyAvailable(bool available)
{
    cutAct->setEnabled(available);
    copyAct->setEnabled(available);
}

void MainWindow::updateModIndicator()
{
    if (activeEditor() && activeEditor()->isModified())
        modLabel->setText(tr("MOD"));
    else
        modLabel->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    workspace->closeAllWindows();
    if (activeEditor())
        event->ignore();
    else
        event->accept();
}
