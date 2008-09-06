#include <qmessagebox.h>

#include "downloader.h"

Downloader::Downloader(const QUrl &url)
{
    if (url.protocol() != "ftp") {
        QMessageBox::warning(0, tr("Downloader"),
                             tr("Protocol must be 'ftp'."));
        emit finished();
        return;
    }

    int port = 21;
    if (url.hasPort())
        port = url.port();

    connect(&ftp, SIGNAL(done(bool)),
            this, SLOT(ftpDone(bool)));
    connect(&ftp, SIGNAL(listInfo(const QUrlInfo &)),
            this, SLOT(listInfo(const QUrlInfo &)));

    ftp.connectToHost(url.host(), port);
    ftp.login(url.user(), url.password());
    ftp.cd(url.path());
    ftp.list();
}

void Downloader::listInfo(const QUrlInfo &urlInfo)
{
    if (urlInfo.isFile() && urlInfo.isReadable()) {
        QFile *file = new QFile(urlInfo.name());
        if (!file->open(IO_WriteOnly)) {
            QMessageBox::warning(0, tr("Downloader"),
                                 tr("Error: Cannot open file "
                                    "%1:\n%2.")
                                 .arg(file->name())
                                 .arg(file->errorString()));
            emit finished();
            return;
        }

        ftp.get(urlInfo.name(), file);
        openedFiles.push_back(file);
    }
}

void Downloader::ftpDone(bool error)
{
    if (error)
        QMessageBox::warning(0, tr("Downloader"),
                             tr("Error: %1.")
                             .arg(ftp.errorString()));

    for (int i = 0; i < (int)openedFiles.size(); ++i)
        delete openedFiles[i];
    emit finished();
}
