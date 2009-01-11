#include <QApplication>
#include "ListWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ListWidget *listWidget1 = new ListWidget;
    listWidget1->setWindowTitle("Head store");
    listWidget1->insertItem(0, new QListWidgetItem(
                   QIcon("caterpillar_head.jpg"), "caterpillar"));
    listWidget1->insertItem(1, new QListWidgetItem(
                   QIcon("momor_head.jpg"), "momor"));
    listWidget1->insertItem(2, new QListWidgetItem(
                   QIcon("bush_head.jpg"), "bush"));
    listWidget1->insertItem(3, new QListWidgetItem(
                   QIcon("bee_head.jpg"), "bee"));
    listWidget1->insertItem(4, new QListWidgetItem(
                   QIcon("cat_head.jpg"), "cat"));
    
    ListWidget *listWidget2 = new ListWidget;
    listWidget2->setWindowTitle("Buy head");
    
    
    listWidget1->show();
    listWidget2->show();
    
    return app.exec();
}
