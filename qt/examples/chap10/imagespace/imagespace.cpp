#include <qdir.h>
#include <qstringlist.h>

#include <iostream>
using namespace std;

int imageSpace(const QString &path)
{
    QDir dir(path);
    QStringList::Iterator it;
    int size = 0;

    QStringList files = dir.entryList("*.png *.jpg *.jpeg",
                                      QDir::Files);
    it = files.begin();
    while (it != files.end()) {
        size += QFileInfo(path, *it).size();
        ++it;
    }

    QStringList dirs = dir.entryList(QDir::Dirs);
    it = dirs.begin();
    while (it != dirs.end()) {
        if (*it != "." && *it != "..")
            size += imageSpace(path + "/" + *it);
        ++it;
    }
    return size;
}

int main(int argc, char *argv[])
{
    QString path = QDir::currentDirPath();
    if (argc > 1)
        path = argv[1];

    cerr << "Space used by images in " << endl
         << path.ascii() << endl
         << "and its subdirectories is "
         << (imageSpace(path) / 1024) << " KB" << endl;

    return 0;
}
