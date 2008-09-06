#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <qfile.h>
#include <qftp.h>
#include <qstringlist.h>
#include <qurl.h>

#include <vector>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(const QUrl &url);

signals:
    void finished();

private slots:
    void ftpDone(bool error);
    void listInfo(const QUrlInfo &urlInfo);

private:
    QFtp ftp;
    std::vector<QFile *> openedFiles;
};

#endif
