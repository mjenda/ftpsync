#include "FtpMirror.h"
#include <QUrl>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <Utilities.h>

FtpMirror::FtpMirror(QObject *parent)
    : QObject(parent)
{
    createConnections();
}

bool FtpMirror::getDictionary(const QUrl &url)
{
    if (not validateUrl(url))
    {
        return false;
    }

    connectToHost(url);
    login(url);
    startDownloading(url);

    return true;
}

void FtpMirror::connectToHost(const QUrl &url)
{
    QTextStream(stdout) << "Connecting to " << url.host() << ":" << url.port(21) << endl;
    m_ftp.connectToHost(url.host(), url.port(21));
}

void FtpMirror::login(const QUrl &url)
{
    QTextStream(stdout) << "Logging as user " << url.userName() << endl;
    m_ftp.login(url.userName(), url.password());
}

void FtpMirror::startDownloading(const QUrl &url)
{
    pendingDirs.append(getStartDir(url));
    processNextDirectory();
}

void FtpMirror::processNextDirectory()
{
    if (pendingDirs.isEmpty())
    {
        emit done();
        return;
    }

    makeNewDirectoryAndCd();

    m_ftp.list();
}

void FtpMirror::processNextFile()
{
    FileUnique file = createDownloadFile();

    if (file.get() == nullptr)
    {
        return;
    }

    QTextStream(stdout) << "GET: " << pendingFilesInCurrDir.back().toUtf8() << endl;

    openedFilesInCurrDir.push_back(std::move(file));
    m_ftp.get(pendingFilesInCurrDir.back(), file.get());
    pendingFilesInCurrDir.removeLast();
}

void FtpMirror::allFtpCommandsDone(bool errorOccured)
{
    if (errorOccured)
    {
        QTextStream(stderr) << "Err: " << qPrintable(m_ftp.errorString()) << endl;
    }

    if (not pendingFilesInCurrDir.empty())
    {
        processNextFile();
    }
    else
    {
        Utilities::dealocate(openedFilesInCurrDir);
        processNextDirectory();
    }
}

void FtpMirror::ftpInformsAboutNewFile(const QUrlInfo &urlInfo)
{
    if (urlInfo.isFile())
    {
        if (urlInfo.isReadable())
        {
            pendingFilesInCurrDir.append(urlInfo.name());
        }
    }
    else if (urlInfo.isDir() &&
             not urlInfo.isSymLink() &&
             Utilities::dirIsNotRefdir(urlInfo.name()))
    {
        pendingDirs.append(currentRemotePath + "/" + urlInfo.name());
    }
}

void FtpMirror::ftpDataTransferProgress(qint64 done, qint64 total)
{
    Utilities::loadbar(done, total);

}

void FtpMirror::createConnections()
{
    connect(this,                         SIGNAL(done()),
            QCoreApplication::instance(), SLOT  (quit()));
    connect(&m_ftp,                       SIGNAL(done(bool)),
            this,                         SLOT(allFtpCommandsDone(bool)));
    connect(&m_ftp,                       SIGNAL(listInfo(const QUrlInfo &)),
            this,                         SLOT(ftpInformsAboutNewFile(const QUrlInfo &)));
    connect(&m_ftp,                       SIGNAL(dataTransferProgress(qint64,qint64)),
            this,                         SLOT(ftpDataTransferProgress(qint64,qint64)));

}

void FtpMirror::makeNewDirectoryAndCd()
{
    currentRemotePath = pendingDirs.takeFirst();
    currentLocalPath = "downloads/" + currentRemotePath;
    QDir(".").mkpath(currentLocalPath);

    QTextStream(stdout) << "CD: " << currentRemotePath << endl;
    m_ftp.cd(currentRemotePath);
}

bool FtpMirror::validateUrl(const QUrl &url)
{
    if (not url.isValid())
    {
        QTextStream(stderr) << "Err: Invalid Url" << endl;
        return false;
    }

    if (url.scheme() != "ftp")
    {
        QTextStream(stderr) << "Err: Url must start with 'ftp:'" << endl;
        return false;
    }

    return true;
}

QString FtpMirror::getStartDir(const QUrl &url)
{
    QString path = url.path();
    if (path.isEmpty())
    {
        path = "/";
    }
    return path;
}

FileUnique FtpMirror::createDownloadFile()
{
    FileUnique file(new QFile(currentLocalPath + "/" + pendingFilesInCurrDir.back()));

    if (not file->open(QIODevice::WriteOnly))
    {
        QTextStream(stderr) << "Err: Cannot write file "
             << qPrintable(QDir::toNativeSeparators(pendingFilesInCurrDir.back()))
             << " : "
             << qPrintable(file->errorString()) << endl;
        return nullptr;
    }
    return std::move(file);
}
