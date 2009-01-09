
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "diagramitem.h"

class DiagramScene;

QT_BEGIN_NAMESPACE
class QAction;
class QToolBox;
class QComboBox;
class QFontComboBox;
class QButtonGroup;
class QGraphicsTextItem;
class QFont;
class QToolButton;
class QAbstractButton;
class QGraphicsView;
QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
   MainWindow();	// 建構子

private slots:
    void backgroundButtonGroupClicked(QAbstractButton *button);	// background其中一個按扭被按下
    void buttonGroupClicked(int id);	// 物件的按扭其中一個被按下
    void deleteItem();			// 刪除物件
    void pointerGroupClicked(int id);	// 設定是點物件還是拉線
    void bringToFront();		// 移動到前面
    void sendToBack();			// 把物件移動到後面
    void itemInserted(DiagramItem *item);	// 插入物件到scene
    void textInserted(QGraphicsTextItem *item);	// 插入文件物件到scene
    void currentFontChanged(const QFont &font);	// 改變文字
    void fontSizeChanged(const QString &size);	// 改變字型大小
    void sceneScaleChanged(const QString &scale);	// 放大縮小
    void textColorChanged();	// 改變文字顏色
    void itemColorChanged();	// 改變物件顏色
    void lineColorChanged();	// 改變線的顏色
    void textButtonTriggered();	// 文字按扭被按
    void fillButtonTriggered();	// 
    void lineButtonTriggered();	// 線的按扭被按
    void handleFontChange();
    void itemSelected(QGraphicsItem *item);	// 物件被選
    void about();

private:
    void createToolBox();
    void createActions();
    void createMenus();
    void createToolbars();
    QWidget *createBackgroundCellWidget(const QString &text,
                                        const QString &image);
    QWidget *createCellWidget(const QString &text, DiagramItem::DiagramType type);
    QMenu *createColorMenu(const char *slot, QColor defaultColor);
    QIcon createColorToolButtonIcon(const QString &image, QColor color);
    QIcon createColorIcon(QColor color);

    DiagramScene *scene;
    QGraphicsView *view;

    QAction *exitAction;
    QAction *addAction;
    QAction *deleteAction;

    QAction *toFrontAction;
    QAction *sendBackAction;
    QAction *aboutAction;

    QMenu *fileMenu;
    QMenu *itemMenu;
    QMenu *aboutMenu;

    QToolBar *textToolBar;
    QToolBar *editToolBar;
    QToolBar *colorToolBar;
    QToolBar *pointerToolbar;

    QComboBox *sceneScaleCombo;
    QComboBox *itemColorCombo;
    QComboBox *textColorCombo;
    QComboBox *fontSizeCombo;
    QFontComboBox *fontCombo;

    QToolBox *toolBox;
    QButtonGroup *buttonGroup;
    QButtonGroup *pointerTypeGroup;
    QButtonGroup *backgroundButtonGroup;
    QToolButton *fontColorToolButton;
    QToolButton *fillColorToolButton;
    QToolButton *lineColorToolButton;
    QAction *boldAction;
    QAction *underlineAction;
    QAction *italicAction;
    QAction *textAction;
    QAction *fillAction;
    QAction *lineAction;
};
//! [0]

#endif
