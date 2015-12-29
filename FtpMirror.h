#ifndef FTPMIRROR_H
#define FTPMIRROR_H

#include <QObject>
#include <FtpClient.h>
#include <QStringList>
#include <memory>
#include <QFile>

using FileUnique = std::unique_ptr<QFile>;

class FtpMirror : QObject
{
Q_OBJECT
public:
    FtpMirror(QObject* parent = 0);
    bool getDictionary(const QUrl &url);

signals:
    void done();

private slots:
    void allFtpCommandsDone(bool errorOccured);
    void ftpInformsAboutNewFile(const QUrlInfo &urlInfo);
    void ftpDataTransferProgress(qint64 done, qint64 total);

private:
    void connectToHost(const QUrl &url);
    void login(const QUrl &url);
    void startDownloading(const QUrl &url);
    void processNextDirectory();
    void processNextFile();
    void createConnections();
    void makeNewDirectoryAndCd();
    bool validateUrl(const QUrl &url);
    QString getStartDir(const QUrl &url);
    FileUnique createDownloadFile();

    FtpClient m_ftp;
    QString currentRemotePath;
    QString currentLocalPath;
    QStringList pendingDirs;
    QStringList pendingFilesInCurrDir;
    std::vector<FileUnique> openedFilesInCurrDir;
};



#endif // FTPMIRROR_H
