#include <QCoreApplication>
#include <QTextStream>
#include <QFtp>
#include <QUrl>
#include <QDebug>
#include <FtpMirror.h>
#include <QTextCodec>


QUrl getUrlFromFile()
{
    QFile userData("../../ftpsync/user.dat");
    if (not userData.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open file.";
        return QUrl();
    }

    QTextStream in(&userData);

    QUrl url(in.readLine());
    url.setUserName(in.readLine());
    url.setPassword(in.readLine());
    url.port(in.readLine().toInt());

    return url;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings( QTextCodec::codecForName("UTF-8") );

    FtpMirror ftp;
    ftp.getDictionary(getUrlFromFile());

    return a.exec();
}

