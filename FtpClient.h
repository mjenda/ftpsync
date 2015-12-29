#ifndef FTPCLIENT_H
#define FTPCLIENT_H
#include <QFtp>

class FtpClient : public QObject
{
Q_OBJECT
public:
    FtpClient(QObject* parent = 0);
    int cd(const QString& dir);
    int list(const QString& dir = QString());
    int connectToHost(const QString& host, quint16 port = 21);
    int login(const QString& user = QString(), const QString& password = QString());
    int get(const QString& file, QIODevice* dev = 0, QFtp::TransferType type = QFtp::Binary);
    QString errorString() const;

signals:
    void done(bool);
    void listInfo(const QUrlInfo &);
    void dataTransferProgress(qint64, qint64);

private:
    QFtp m_ftpImpl;
};

#endif // FTPCLIENT_H
