#include <FtpMirrorPoco.h>
#include <FtpData.h>
#include <iostream>
#include <fstream>

int main()
{
    std::ifstream dataFile;
    dataFile.open("user.dat");
    FtpData ftpData;
    std::getline(dataFile, ftpData.host);
    std::getline(dataFile, ftpData.dir);
    std::getline(dataFile, ftpData.uname);
    std::getline(dataFile, ftpData.password);

    FtpMirrorPoco ftpClient(ftpData);
    ftpClient.getDictionary();

    dataFile.close();
}

