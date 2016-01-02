#ifndef FTPMIRROR_H
#define FTPMIRROR_H

#include <QObject>
#include <FtpClient.h>
#include <QStringList>
#include <memory>
#include <QFile>
#include <QFileInfo>

class FileAutoSettingModifyTime
{
public:
    FileAutoSettingModifyTime(const QString& filePath, const QDateTime& modifyTimeToSet = {})
        : file(filePath), modifyTime(modifyTimeToSet)
    {
    }

    QFile * getFile()
    {
        return &file;
    }

    ~FileAutoSettingModifyTime();

private:
    QFile file;
    QDateTime modifyTime;
};

using FileUnique = std::unique_ptr<FileAutoSettingModifyTime>;

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
    void closeOpenedFile();
    QFileInfoList getInfoAboutLocalFile(const QString& fileName);
    bool fileNeedsToBeDownloaded(const QFileInfoList &localFile, const QUrlInfo &urlInfo);
    QString getStartDir(const QUrl &url);
    FileUnique createLocalDownloadFile();

    FtpClient m_ftp;
    QString currentRemotePath;
    QString currentLocalPath;
    QStringList pendingDirs;
    QList<QUrlInfo> pendingFilesInCurrDir;
    FileUnique openedFile;
};



#endif // FTPMIRROR_H
