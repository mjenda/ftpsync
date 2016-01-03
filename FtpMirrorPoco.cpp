#include "FtpMirrorPoco.h"
#include <Poco/Net/FTPClientSession.h>
#include <iostream>
#include <fstream>
#include <FtpData.h>
#include <sstream>
#include <algorithm>
#include <ftpparse.h>
#include <cstring>
#include <stdio.h>
#include <pthread.h>
#include "Poco/StreamCopier.h"
#include "Poco/File.h"

FtpMirrorPoco::FtpMirrorPoco(const FtpData& ftpData)
    : session(ftpData.host)
{
    session.setTimeout(Poco::Timespan(0, 0, 1, 0, 0));
    session.login(ftpData.uname, ftpData.password);
    session.setWorkingDirectory(ftpData.dir);
}

bool FtpMirrorPoco::getDictionary(const std::string& dirPath)
{
    dirsLeftToDownload.clear();
    dirsLeftToDownload.push_back(dirPath);

    while (not dirsLeftToDownload.empty())
    {
        currDir = dirsLeftToDownload.back();
        dirsLeftToDownload.pop_back();
        FileInfoList fileListToDownload = getFileListFromDirectory(currDir);
        downloadFilesFromCurrentDirectory(fileListToDownload);
    }

    return 0;
}

FileInfoList FtpMirrorPoco::getFileListFromDirectory(const std::string &dirPath)
{
    std::cout << "ENTERING DIRECTORY : " << dirPath << std::endl;
    session.setWorkingDirectory(dirPath);

    std::istream& fileListStream = session.beginList("", true);

    FileInfoList fileList;
    std::string line;
    while (std::getline(fileListStream, line))
    {
        ParsedFtpLine parsedLine = parseFtpListLine(line);

        if (parsedLine.isDir() && not parsedLine.isDotOrDotDotDir())
        {
            //std::cout << parsedLine.toStr();

            std::cout << "ADDING DIR: " << dirPath + parsedLine.name << std::endl;

            dirsLeftToDownload.push_back(dirPath + parsedLine.name + "/");
        }
        else if (parsedLine.isFile())
        {
            fileList.push_back(parsedLine);
        }
    }
    session.endList();
    return fileList;
}

FtpMirrorPoco::~FtpMirrorPoco()
{
    session.close();
}

bool FtpMirrorPoco::isDotOrDotDotDirectory(const std::string &file)
{
    if (file[0] == '.')
    {
        if (file.size() == 1)
        {
            return true;
        }
        else if (file.size() == 2 && file[1] == '.')
        {
            return true;
        }
    }
    return false;
}

ParsedFtpLine FtpMirrorPoco::parseFtpListLine(const std::string &line)
{
    struct ftpparse parsedLine;
    unsigned int sizeWithoutNewLine = line.size()-1;
    char c_strLine[sizeWithoutNewLine];
    strcpy(c_strLine, line.c_str());
    ftpparse(&parsedLine, c_strLine, sizeWithoutNewLine);
    ParsedFtpLine parsedFtpLine(parsedLine);
    return parsedFtpLine;
}

void FtpMirrorPoco::endWithSlash(std::string &path)
{
    if (path[path.length()-1] != '/')
    {
        path += "/";
    }
}

void FtpMirrorPoco::downloadFilesFromCurrentDirectory(const FileInfoList &fileList)
{
    for (auto file : fileList)
    {
        std::cout << "DOWNLOADING FILE : " << file.name << std::endl;
        session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);
        std::istream& is = session.beginDownload(file.name);
        std::ofstream ofs;

        Poco::File folder("." + currDir);
        if (!folder.exists())
        {
            folder.createDirectory();
        }
        ofs.open("." + currDir + "/" + file.name, std::ofstream::out | std::ofstream::app | std::ofstream::binary);

        Poco::StreamCopier::copyStream(is, ofs);
        session.endDownload();
        std::cout << "END OF DOWNLOADING FILE : " << file.name << std::endl;
    }
}
