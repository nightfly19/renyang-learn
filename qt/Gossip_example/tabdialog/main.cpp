#include <qapplication.h>
#include <qtabdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlineedit.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    QTabDialog dialog;
    QWidget *page = new QWidget();

    dialog.resize(0, 200);
    new QLabel("Label here", page);  // 置入元件至QWidget
    dialog.insertTab(page, "Page &1");  // 將QWidget插入一個標籤頁

    page = new QWidget();
    new QPushButton("Button here", page);
    dialog.addTab(page, "Page &2");

    page = new QWidget();
    new QLineEdit("Linedit here", page);
    dialog.addTab(page, "Page &3");

     // 功能按鈕顯示
    dialog.setDefaultButton("[Default]");
    dialog.setHelpButton("[Help]");
    dialog.setApplyButton();
    dialog.setCancelButton();

    QObject::connect(&dialog, SIGNAL(applyButtonPressed()),
                     qApp, SLOT(quit()));

    app.setMainWidget(&dialog);
    dialog.show();

    return app.exec();
}
