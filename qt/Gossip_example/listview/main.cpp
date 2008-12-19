#include <qapplication.h>
#include <qlistview.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qpixmap.h>

void dirlist(QListViewItem*, QFileInfo*);

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    QListView *root = new QListView();
    root->addColumn("Name");
    root->addColumn("Size");

    QDir d = QDir::home();
    d.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks);
    d.setSorting(QDir::DirsFirst | QDir::Name);

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    QListViewItem *child;

    while((fi = it.current()) != 0) {
        if ( fi->fileName() == "." || fi->fileName() == ".." );
        else if(fi->isDir()) {
            child = new QListViewItem(root, fi->fileName().latin1(), "dir");
            child->setPixmap(0, QPixmap( "dirclosed.xpm" ));
            dirlist(child, fi);
        }
        else {
            child = new QListViewItem(root,
                    fi->fileName().latin1(), QString::number(fi->size()));
            child->setPixmap(0, QPixmap( "file.xpm" ));
        }
        ++it;
    }
   
    app.setMainWidget(root);
    root->show();

    return app.exec();
}

void dirlist(QListViewItem *parent, QFileInfo *pfi) {
    QDir d;

    d.setPath(pfi->filePath());
    d.setFilter(QDir::Files | QDir::Dirs | QDir::NoSymLinks);
    d.setSorting(QDir::DirsFirst | QDir::Name);

    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it(*list);
    QFileInfo *fi;
    QListViewItem *child;

    while((fi = it.current()) != 0) {
        if ( fi->fileName() == "." || fi->fileName() == ".." );
        else if(fi->isDir()) {
            child = new QListViewItem(parent, fi->fileName().latin1(), "dir");
            child->setPixmap(0, QPixmap("dirclosed.xpm" ));
            dirlist(child, fi);
        }
        else {
            child = new QListViewItem(parent,
                    fi->fileName().latin1(), QString::number(fi->size()));
            child->setPixmap(0, QPixmap("file.xpm" ));
        }
        ++it;
    }
}
