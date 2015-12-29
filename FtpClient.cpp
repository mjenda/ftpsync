#include "FtpClient.h"

FtpClient::FtpClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_ftpImpl, SIGNAL(done(bool)),
            this,       SIGNAL(done(bool)));
    connect(&m_ftpImpl, SIGNAL(listInfo(const QUrlInfo &)),
            this,       SIGNAL(listInfo(const QUrlInfo &)));
    connect(&m_ftpImpl, SIGNAL(dataTransferProgress(qint64,qint64)),
            this,       SIGNAL(dataTransferProgress(qint64,qint64)));
}

int FtpClient::cd(const QString& dir)
{
    return m_ftpImpl.cd(dir);
}

int FtpClient::list(const QString& dir)
{
    return m_ftpImpl.list(dir);
}

int FtpClient::connectToHost(const QString& host, quint16 port)
{
    return m_ftpImpl.connectToHost(host, port);
}

int FtpClient::login(const QString& user, const QString& password)
{
    return m_ftpImpl.login(user, password);
}

int FtpClient::get(const QString& file, QIODevice* dev, QFtp::TransferType type)
{
    return m_ftpImpl.get(file, dev, type);
}

QString FtpClient::errorString() const
{
    return m_ftpImpl.errorString();
}
