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
    auto oneSecondTimeout = Poco::Timespan(0, 0, 1, 0, 0);
    session.setTimeout(oneSecondTimeout);
    session.login(ftpData.uname, ftpData.password);
    session.setWorkingDirectory(ftpData.dir);
}

bool FtpMirrorPoco::getDictionary(const std::string& dirPath)
{
    dirsLeftToDownload.clear();
    dirsLeftToDownload.push_back(dirPath);

    while (not dirsLeftToDownload.empty())
    {
        selectNextDirToProcess();
        auto fileListToDownload = getFileListFromCurrentDirectory();
        downloadFilesFromCurrentDirectory(fileListToDownload);
    }

    return 0;
}

FileInfoList FtpMirrorPoco::getFileListFromCurrentDirectory()
{
    std::istream& fileListStream = session.beginList("", true);

    FileInfoList fileList;
    std::string line;
    while (std::getline(fileListStream, line))
    {
        processLine(fileList, line);
    }

    session.endList();
    return fileList;
}

FtpMirrorPoco::~FtpMirrorPoco()
{
    session.close();
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

void FtpMirrorPoco::downloadFilesFromCurrentDirectory(const FileInfoList &fileList)
{
    for (auto file : fileList)
    {
        downloadOneFileFromCurrentDirectory(file);
    }
}

void FtpMirrorPoco::downloadOneFileFromCurrentDirectory(const FileInfo &fileInfo)
{
    std::cout << "DOWNLOADING FILE : " << fileInfo.name << std::endl;

    session.setFileType(Poco::Net::FTPClientSession::TYPE_BINARY);
    auto& inputFileStream = session.beginDownload(fileInfo.name);

    createCurrentDirectoryInLocalFileSystem();

    std::ofstream localFileStream;
    initLocalFileStream(localFileStream, fileInfo);

    Poco::StreamCopier::copyStream(inputFileStream, localFileStream);

    session.endDownload();
    localFileStream.close();

    setFileModifiedTimeToServerOne(fileInfo);

    std::cout << "END OF DOWNLOADING FILE : " << fileInfo.name << std::endl;
}

void FtpMirrorPoco::createCurrentDirectoryInLocalFileSystem()
{
    Poco::File folder(makePathRelative(currDir));
    if (not folder.exists())
    {
        folder.createDirectory();
    }
}

void FtpMirrorPoco::initLocalFileStream(std::ofstream& localFileStream, const FileInfo &fileInfo)
{
    localFileStream.open(makePathRelative(endPathWithSlash(currDir)) + fileInfo.name,
             std::ofstream::out | std::ofstream::app | std::ofstream::binary);
}

void FtpMirrorPoco::selectNextDirToProcess()
{
    currDir = dirsLeftToDownload.back();
    dirsLeftToDownload.pop_back();
    std::cout << "ENTERING DIRECTORY : " << currDir << std::endl;
    session.setWorkingDirectory(currDir);
}

void FtpMirrorPoco::processLine(FileInfoList& fileList, const std::string& line)
{
    FileInfo parsedLine = parseFtpListLine(line);

    if (parsedLine.isDir() && not parsedLine.isDotOrDotDotDir())
    {
        //std::cout << parsedLine.toStr();

        std::cout << "ADDING DIR: " << currDir + parsedLine.name << std::endl;

        dirsLeftToDownload.push_back(currDir + parsedLine.name + "/");
    }
    else if (parsedLine.isFile() && fileHasChanged(parsedLine))
    {
        fileList.push_back(parsedLine);
    }
}

void FtpMirrorPoco::setFileModifiedTimeToServerOne(const FileInfo &fileInfo)
{
    Poco::File file(makePathRelative(endPathWithSlash(currDir)) + fileInfo.name);
    if (file.exists())
    {
        file.setLastModified(Poco::Timestamp::fromEpochTime(fileInfo.modifiedTime));
    }
}

bool FtpMirrorPoco::fileHasChanged(const FileInfo &fileInfo)
{
    Poco::File file(makePathRelative(endPathWithSlash(currDir)) + fileInfo.name);
    if (not file.exists())
    {
        return true;
    }

    if (file.getLastModified().epochTime() == fileInfo.modifiedTime)
    {
        std::cout << "FILE " << fileInfo.name  << " HASNT CHANGED!" << std::endl;
        return false;
    }
    else
    {
        std::cout << "FILE " << fileInfo.name  << " HAS CHANGED!" << std::endl;
        return true;
    }

}

std::string & FtpMirrorPoco::endPathWithSlash(std::string &path)
{
    if (path[path.length()-1] != '/')
    {
        path += "/";
    }
    return path;
}

std::string & FtpMirrorPoco::makePathRelative(std::string &path)
{
    if (path[0] != '.')
    {
        path = "." + path;
    }
    return path;
}
