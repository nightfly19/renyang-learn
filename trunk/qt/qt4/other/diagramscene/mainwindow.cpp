
#include <QtGui>
#include <QLabel>

#include "mainwindow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "diagramtextitem.h"

const int InsertTextButton = 10;

//! [0]
MainWindow::MainWindow()
{
    createActions();	// 第一個部分建立Action
    createToolBox();	// 第二個部分建立ToolBox
    createMenus();	// 第三個部分建立Menu

    // 第四個部分建立CentralWidget
    scene = new DiagramScene(itemMenu);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, SIGNAL(itemInserted(DiagramItem *)),		// 插入scene的圖行物件
            this, SLOT(itemInserted(DiagramItem *)));
    connect(scene, SIGNAL(textInserted(QGraphicsTextItem *)),	// 插入scene的文字物件
        this, SLOT(textInserted(QGraphicsTextItem *)));
    connect(scene, SIGNAL(itemSelected(QGraphicsItem *)),	// scene的物件被選擇
        this, SLOT(itemSelected(QGraphicsItem *)));
    createToolbars();	// 第四個部分建立toolbar

    QHBoxLayout *layout = new QHBoxLayout;	// 設定一個水平的layout
    layout->addWidget(toolBox);			// 先在layout的左邊加入一個toolBox
    view = new QGraphicsView(scene);		// 在layout右邊加入一個scene
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);			// 把設定好的layout去管理此widget

    setCentralWidget(widget);			// 設定主視窗是widget
    setWindowTitle(tr("Diagramscene"));
    // 到目前為止,MainWindow的centralwidget是由toolbox與scene所主成:而這是第五個部分
    
    // 第六個部分設定狀態列
    QStatusBar *statusBar = this->statusBar();
    statusBar->showMessage("Status here...");

    // 第七個部分設定Dock Widget
}
//! [0]

//! [1]
void MainWindow::backgroundButtonGroupClicked(QAbstractButton *button)
{
    QList<QAbstractButton *> buttons = backgroundButtonGroup->buttons();
    foreach (QAbstractButton *myButton, buttons) {
    if (myButton != button)
        button->setChecked(false);
    }
    QString text = button->text();
    if (text == tr("Blue Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background1.png"));
    else if (text == tr("White Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background2.png"));
    else if (text == tr("Gray Grid"))
        scene->setBackgroundBrush(QPixmap(":/images/background3.png"));
    else
        scene->setBackgroundBrush(QPixmap(":/images/background4.png"));

    scene->update();
    view->update();
}
//! [1]

//! [2]
void MainWindow::buttonGroupClicked(int id)
{
    QList<QAbstractButton *> buttons = buttonGroup->buttons();	// 收集所有此GroupButton的按扭
    foreach (QAbstractButton *button, buttons) {	// 把buttons內收集的抓出來存到*button
    if (buttonGroup->button(id) != button)	// 把所有的此GroupButton內的button掃過一次,是此次的button則把它回復沒有按的狀態
        button->setChecked(false);
    }
    if (id == InsertTextButton) {		// 若被按的是代表要插入文字的按扭
        scene->setMode(DiagramScene::InsertText);
    } else {
        scene->setItemType(DiagramItem::DiagramType(id));
        scene->setMode(DiagramScene::InsertItem);	// 設定Scene的模式:InsertItem, InsertLine, InsertText, MoveItem
    }
}
//! [2]

//! [3]
void MainWindow::deleteItem()
{
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == DiagramItem::Type) {
            qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
        }
        scene->removeItem(item);
    }
}
//! [3]

//! [4]
void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));	// 設定scene的模式是pointer或是拉線
}
//! [4]

//! [5]
void MainWindow::bringToFront()
{
    if (scene->selectedItems().isEmpty())	// 找出被點選的物件
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() >= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() + 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [5]

//! [6]
void MainWindow::sendToBack()
{
    if (scene->selectedItems().isEmpty())
        return;

    QGraphicsItem *selectedItem = scene->selectedItems().first();
    QList<QGraphicsItem *> overlapItems = selectedItem->collidingItems();

    qreal zValue = 0;
    foreach (QGraphicsItem *item, overlapItems) {
        if (item->zValue() <= zValue &&
            item->type() == DiagramItem::Type)
            zValue = item->zValue() - 0.1;
    }
    selectedItem->setZValue(zValue);
}
//! [6]

//! [7]
void MainWindow::itemInserted(DiagramItem *item)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));	// 依目前被按下的按扭,設定scene的mode
    buttonGroup->button(int(item->diagramType()))->setChecked(false);	// 設定按扭跳回來
}
//! [7]

//! [8]
void MainWindow::textInserted(QGraphicsTextItem *)
{
    buttonGroup->button(InsertTextButton)->setChecked(false);	// 依目前被按下的按扭,設定scene的mode
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}
//! [8]

//! [9]
void MainWindow::currentFontChanged(const QFont &)
{
    handleFontChange();
}
//! [9]

//! [10]
void MainWindow::fontSizeChanged(const QString &)
{
    handleFontChange();
}
//! [10]

//! [11]
void MainWindow::sceneScaleChanged(const QString &scale)
{
    double newScale = scale.left(scale.indexOf(tr("%"))).toDouble() / 100.0;
    QMatrix oldMatrix = view->matrix();
    view->resetMatrix();	// 設定回一個identify的matrix,反正,就是先還原啦,設定為原本的大小
    view->translate(oldMatrix.dx(), oldMatrix.dy());	// 設定回原本的偏移值
    view->scale(newScale, newScale);	// 設定放大比例
}
//! [11]

//! [12]
void MainWindow::textColorChanged()
{
    textAction = qobject_cast<QAction *>(sender());
    fontColorToolButton->setIcon(createColorToolButtonIcon(
                ":/images/textpointer.png",
                qVariantValue<QColor>(textAction->data())));
    textButtonTriggered();
}
//! [12]

//! [13]
void MainWindow::itemColorChanged()
{
    fillAction = qobject_cast<QAction *>(sender());
    fillColorToolButton->setIcon(createColorToolButtonIcon(
                 ":/images/floodfill.png",
                 qVariantValue<QColor>(fillAction->data())));
    fillButtonTriggered();
}
//! [13]

//! [14]
void MainWindow::lineColorChanged()
{
    lineAction = qobject_cast<QAction *>(sender());
    lineColorToolButton->setIcon(createColorToolButtonIcon(
                 ":/images/linecolor.png",
                 qVariantValue<QColor>(lineAction->data())));
    lineButtonTriggered();
}
//! [14]

//! [15]
void MainWindow::textButtonTriggered()
{
    scene->setTextColor(qVariantValue<QColor>(textAction->data()));	// 設定scene的text顏色
}
//! [15]

//! [16]
void MainWindow::fillButtonTriggered()
{
    scene->setItemColor(qVariantValue<QColor>(fillAction->data()));
}
//! [16]

//! [17]
void MainWindow::lineButtonTriggered()
{
    scene->setLineColor(qVariantValue<QColor>(lineAction->data()));
}
//! [17]

//! [18]
void MainWindow::handleFontChange()
{
    QFont font = fontCombo->currentFont();
    font.setPointSize(fontSizeCombo->currentText().toInt());	// 取出combo內的字串,把它轉成數字
    font.setWeight(boldAction->isChecked() ? QFont::Bold : QFont::Normal);	// 查看bold是否有被按下
    font.setItalic(italicAction->isChecked());			// 是否斜體
    font.setUnderline(underlineAction->isChecked());		// 底線

    scene->setFont(font);		// 設定以後在scene的字型的樣式
}
//! [18]

//! [19]
void MainWindow::itemSelected(QGraphicsItem *item)
{
    qDebug("itemSelected");
    DiagramTextItem *textItem =
    qgraphicsitem_cast<DiagramTextItem *>(item);

    QFont font = textItem->font();
    QColor color = textItem->defaultTextColor();
    fontCombo->setCurrentFont(font);
    fontSizeCombo->setEditText(QString().setNum(font.pointSize()));
    boldAction->setChecked(font.weight() == QFont::Bold);
    italicAction->setChecked(font.italic());
    underlineAction->setChecked(font.underline());
}
//! [19]

//! [20]
void MainWindow::about()
{
    QMessageBox::about(this, tr("About Diagram Scene"),
                       tr("The <b>Diagram Scene</b> example shows "
                          "use of the graphics framework."));
}
//! [20]

//! [21]
void MainWindow::createToolBox()
{
    buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive(false);
    connect(buttonGroup, SIGNAL(buttonClicked(int)),	// 當按扭被按時,參數是button的id
            this, SLOT(buttonGroupClicked(int)));
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(createCellWidget(tr("Conditional"),	// 參數:(QToolButton,row,column)
                               DiagramItem::Conditional), 0, 0);
    layout->addWidget(createCellWidget(tr("Process"),
                      DiagramItem::Step),0, 1);
    layout->addWidget(createCellWidget(tr("Input/Output"),
                      DiagramItem::Io), 1, 0);
//! [21]			// 要把文字按扭與圖型按扭分開是因為在scene內圖型按扭有menu而文字按扭沒有

    QToolButton *textButton = new QToolButton;
    textButton->setCheckable(true);
    buttonGroup->addButton(textButton, InsertTextButton);	// 設定要如入QButtonGroup的按扭與其id
    textButton->setIcon(QIcon(QPixmap(":/images/textpointer.png")
                        .scaled(30, 30,Qt::KeepAspectRatio)));
    textButton->setIconSize(QSize(50, 50));			// 主要是用這個來設定QToolButton的大小
    QGridLayout *textLayout = new QGridLayout;
    textLayout->addWidget(textButton, 0, 0, Qt::AlignHCenter);
    textLayout->addWidget(new QLabel(tr("Text")), 1, 0, Qt::AlignCenter);
    QWidget *textWidget = new QWidget;
    textWidget->setLayout(textLayout);
    layout->addWidget(textWidget, 1, 1);

    layout->setRowStretch(2, 10);	// 如此設定可以讓QToolBox下面有空間
    layout->setColumnStretch(2, 10);	// 如此設定可以讓QToolBox右邊有空間

    QWidget *itemWidget = new QWidget;	// 設定一個widget來放置以上建立的button
    itemWidget->setLayout(layout);

    backgroundButtonGroup = new QButtonGroup;	// 建立一個ButtonGroup for backgroundButton
    connect(backgroundButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),	// 當有其中一個被按,則執行backgroundButtonClicked()
            this, SLOT(backgroundButtonGroupClicked(QAbstractButton *)));	// 並且這裡特地用另一種參數來傳遞

    QGridLayout *backgroundLayout = new QGridLayout;
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Blue Grid"),
                ":/images/background1.png"), 0, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("White Grid"),
                ":/images/background2.png"), 0, 1);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("Gray Grid"),
                    ":/images/background3.png"), 1, 0);
    backgroundLayout->addWidget(createBackgroundCellWidget(tr("No Grid"),
                ":/images/background4.png"), 1, 1);

    backgroundLayout->setRowStretch(2, 10);
    backgroundLayout->setColumnStretch(2, 10);

    QWidget *backgroundWidget = new QWidget;
    backgroundWidget->setLayout(backgroundLayout);


//! [22]
    toolBox = new QToolBox;
    toolBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));	// QSizePolicy(水平,垂直)
    toolBox->setMinimumWidth(itemWidget->sizeHint().width());	// 重點:設定寬度最小為itemWidget提示的寬度
    toolBox->addItem(itemWidget, tr("Basic Flowchart Shapes"));	// 設定第一個page
    toolBox->addItem(backgroundWidget, tr("Backgrounds"));	// 設定第二個page
}
//! [22]

//! [23]
void MainWindow::createActions()
{
    // 設定QAction主要分成這四個步驟
    toFrontAction = new QAction(QIcon(":/images/bringtofront.png"),	// 設定圖片與名稱
                                tr("Bring to &Front"), this);
    toFrontAction->setShortcut(tr("Ctrl+F"));				// 設定快捷鍵
    toFrontAction->setStatusTip(tr("Bring item to front"));		// 顯示在狀態列中,但是,因為此例子沒有狀態列,所以看不出結果!!
    connect(toFrontAction, SIGNAL(triggered()),				// 當toFrontAction被觸發，則執行bringToFront()
            this, SLOT(bringToFront()));
    // 以上只是建立Action而以,還沒有加入到QToolbar和QToolMenu中
//! [23]

    sendBackAction = new QAction(QIcon(":/images/sendtoback.png"),
                                 tr("Send to &Back"), this);
    sendBackAction->setShortcut(tr("Ctrl+B"));
    sendBackAction->setStatusTip(tr("Send item to back"));
    connect(sendBackAction, SIGNAL(triggered()),
        this, SLOT(sendToBack()));

    deleteAction = new QAction(QIcon(":/images/delete.png"),
                               tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()),
        this, SLOT(deleteItem()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+X"));
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    boldAction = new QAction(tr("Bold"), this);	// 設定粗體按扭的QAction
    boldAction->setCheckable(true);
    QPixmap pixmap(":/images/bold.png");
    boldAction->setIcon(QIcon(pixmap));
    boldAction->setShortcut(tr("Ctrl+B"));
    connect(boldAction, SIGNAL(triggered()),
            this, SLOT(handleFontChange()));

    italicAction = new QAction(QIcon(":/images/italic.png"),	// 設定斜體按扭的QAction
                               tr("Italic"), this);
    italicAction->setCheckable(true);
    italicAction->setShortcut(tr("Ctrl+I"));
    connect(italicAction, SIGNAL(triggered()),
            this, SLOT(handleFontChange()));

    underlineAction = new QAction(QIcon(":/images/underline.png"),	// 設定底線按扭的QAction
                                  tr("Underline"), this);
    underlineAction->setCheckable(true);
    underlineAction->setShortcut(tr("Ctrl+U"));
    connect(underlineAction, SIGNAL(triggered()),
            this, SLOT(handleFontChange()));

    aboutAction = new QAction(tr("A&bout"), this);		// 設定About的QAction
    aboutAction->setShortcut(tr("Ctrl+B"));
    connect(aboutAction, SIGNAL(triggered()),
            this, SLOT(about()));
}

//! [24]
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));	// 先加入一個最上層的QMenu
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));	// 再加入一個最上層的QMenu
    itemMenu->addAction(deleteAction);
    itemMenu->addSeparator();			// 在QMenu加教一個分格線
    itemMenu->addAction(toFrontAction);		// 實際上把建立好的QAction放到QMenuBar中,當它按下時,就是觸發triggered()
    itemMenu->addAction(sendBackAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));// 再加入一個最上層的QMenu
    aboutMenu->addAction(aboutAction);
}
//! [24]

//! [25]
void MainWindow::createToolbars()
{
//! [25]
    editToolBar = addToolBar(tr("Edit"));	// 這是橫的那一條
    editToolBar->addAction(deleteAction);
    editToolBar->addAction(toFrontAction);	// 實際上把建立好的QAction放到QToolBar中,當它按下時,就是觸發triggered()
    editToolBar->addAction(sendBackAction);

    fontCombo = new QFontComboBox();
    connect(fontCombo, SIGNAL(currentFontChanged(const QFont &)),	// 用下拉選單拉新的字型
            this, SLOT(currentFontChanged(const QFont &)));

    fontSizeCombo = new QComboBox;	// 建立一個下拉選單,裡面的值是8,10,12...28
    fontSizeCombo->setEditable(true);
    for (int i = 8; i < 30; i = i + 2)
        fontSizeCombo->addItem(QString().setNum(i));	// 把數字轉換成字串
    QIntValidator *validator = new QIntValidator(2, 64, this);	// 整數驗證最小2,最大是64
    fontSizeCombo->setValidator(validator);	// 設定此欄位為只能輸入2到64的整數
    connect(fontSizeCombo, SIGNAL(currentIndexChanged(const QString &)),	// 當有改變時,改變目前系統的字型font
            this, SLOT(fontSizeChanged(const QString &)));

    fontColorToolButton = new QToolButton;
    fontColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);	// 設定QToolButton的下拉式選單
    fontColorToolButton->setMenu(createColorMenu(SLOT(textColorChanged()),	// SLOT(textColorChanged())Qt會把它轉成字串
                                                 Qt::black));
    textAction = fontColorToolButton->menu()->defaultAction();	// 取得defaultAction，並且設定QToolBar的menu
    fontColorToolButton->setIcon(createColorToolButtonIcon(
    ":/images/textpointer.png", Qt::black));
    fontColorToolButton->setAutoFillBackground(true);
    connect(fontColorToolButton, SIGNAL(clicked()),
            this, SLOT(textButtonTriggered()));

//! [26]
    fillColorToolButton = new QToolButton;
    fillColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    fillColorToolButton->setMenu(createColorMenu(SLOT(itemColorChanged()),
                         Qt::white));
    fillAction = fillColorToolButton->menu()->defaultAction();
    fillColorToolButton->setIcon(createColorToolButtonIcon(
    ":/images/floodfill.png", Qt::white));
    connect(fillColorToolButton, SIGNAL(clicked()),
            this, SLOT(fillButtonTriggered()));
//! [26]

    lineColorToolButton = new QToolButton;
    lineColorToolButton->setPopupMode(QToolButton::MenuButtonPopup);
    lineColorToolButton->setMenu(createColorMenu(SLOT(lineColorChanged()),
                                 Qt::black));
    lineAction = lineColorToolButton->menu()->defaultAction();
    lineColorToolButton->setIcon(createColorToolButtonIcon(
        ":/images/linecolor.png", Qt::black));
    connect(lineColorToolButton, SIGNAL(clicked()),
            this, SLOT(lineButtonTriggered()));

    textToolBar = addToolBar(tr("Font"));
    textToolBar->addWidget(fontCombo);
    textToolBar->addWidget(fontSizeCombo);
    textToolBar->addAction(boldAction);
    textToolBar->addAction(italicAction);
    textToolBar->addAction(underlineAction);

    colorToolBar = addToolBar(tr("Color"));
    colorToolBar->addWidget(fontColorToolButton);
    colorToolBar->addWidget(fillColorToolButton);
    colorToolBar->addWidget(lineColorToolButton);

    QToolButton *pointerButton = new QToolButton;	// 準備要設定點選物件的方式
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *linePointerButton = new QToolButton;	// 設定為拉線的方式
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/linepointer.png"));

    pointerTypeGroup = new QButtonGroup;	// 設定此兩個按扭是一個GroupButton
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(linePointerButton,
                                int(DiagramScene::InsertLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    sceneScaleCombo = new QComboBox;
    QStringList scales;
    scales << tr("50%") << tr("75%") << tr("100%") << tr("125%") << tr("150%");
    sceneScaleCombo->addItems(scales);
    sceneScaleCombo->setCurrentIndex(2);
    connect(sceneScaleCombo, SIGNAL(currentIndexChanged(const QString &)),
            this, SLOT(sceneScaleChanged(const QString &)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(sceneScaleCombo);
//! [27]
}
//! [27]

//! [28]
QWidget *MainWindow::createBackgroundCellWidget(const QString &text,
                        const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}
//! [28]

//! [29]
QWidget *MainWindow::createCellWidget(const QString &text,	// 建立一個在QToolBox內的按扭,依不同的種類
                      DiagramItem::DiagramType type)
{

    DiagramItem item(type, itemMenu);	// 自己設定此物件的視別碼,此物件的類別是我們自己建立的,和對它按右鍵的menu
    QIcon icon(item.image());		// 傳回此物件的icon;依此種物件,決定要使用的圖示

    QToolButton *button = new QToolButton;
    button->setIcon(icon);
    button->setIconSize(QSize(50, 50));	// 由此決定按扭的大小
    button->setCheckable(true);
    buttonGroup->addButton(button, int(type));	// 加入到這一個QGroupButton中,並且設定此按扭的id

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);	// 此widget是由QToolButton與QLabel所組成的

    return widget;
}
//! [29]

//! [30]
QMenu *MainWindow::createColorMenu(const char *slot, QColor defaultColor)	// 建立一個QToolBox的下拉式選單,SLOT的對應參數必需是const char *
{
    QList<QColor> colors;
    colors << Qt::black << Qt::white << Qt::red << Qt::blue << Qt::yellow;
    QStringList names;
    names << tr("black") << tr("white") << tr("red") << tr("blue")
          << tr("yellow");

    QMenu *colorMenu = new QMenu;
    for (int i = 0; i < colors.count(); ++i) {
        QAction *action = new QAction(names.at(i), this);
        action->setData(colors.at(i));	// 把資料塞進去此QAction
        action->setIcon(createColorIcon(colors.at(i)));
        connect(action, SIGNAL(triggered()),
                this, slot);	// 這裡的slot代表的就是SLOT(textColorChanged()),所以,當action被觸發時,本類別的textColorChanged()就會被執行
        colorMenu->addAction(action);
        if (colors.at(i) == defaultColor) {
            colorMenu->setDefaultAction(action);	// 要設定default是因為這是一個QToolButton的Menu
        }
    }
    return colorMenu;
}
//! [30]

//! [31]
QIcon MainWindow::createColorToolButtonIcon(const QString &imageFile,
                        QColor color)
{
    QPixmap pixmap(50, 80);		// 設定一個pixmap的大小為50x80
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    QPixmap image(imageFile);		// 圖片路徑
    QRect target(0, 0, 50, 60);		// 因為QRect自己並沒有小座標,所以(0,0)是表示要畫在device的座標
    QRect source(0, 0, 42, 42);		// 因該是圖片剛好是42x42
    painter.fillRect(QRect(0, 60, 50, 80), color);
    painter.drawPixmap(target, image, source);	// 由image中取一部分source設定的出來畫在target中
    						// 另外,由image取出的42x42會縮放到與target一樣的大小

    return QIcon(pixmap);
}
//! [31]

//! [32]
QIcon MainWindow::createColorIcon(QColor color)
{
    QPixmap pixmap(20, 20);	// 建立一個20x20的pixmap
    QPainter painter(&pixmap);	// 畫東西在此pixmap上
    painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);	// 用此顏色把它填滿

    return QIcon(pixmap);
}
//! [32]
