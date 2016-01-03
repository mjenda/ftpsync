#include "FtpMirrorPoco.h"
#include <Poco/Net/FTPClientSession.h>
#include <iostream>
#include <fstream>
#include <FtpData.h>
#include <sstream>
#include <algorithm>
#include <ftpparse.h>
#include <cstring>

FtpMirrorPoco::FtpMirrorPoco(const FtpData& ftpData)
    : session(ftpData.host)
{
    session.setTimeout(Poco::Timespan(0, 0, 1, 0, 0));
    session.login(ftpData.uname, ftpData.password);
    session.setWorkingDirectory(ftpData.dir);
}

bool FtpMirrorPoco::getDictionary()
{
    getFileListFromDirectory("/test/");

    return 0;
}

StringList FtpMirrorPoco::getFileListFromDirectory(const std::string &dirPath)
{
    session.setWorkingDirectory(dirPath);

    std::istream& fileListStream = session.beginList("", true);

    StringList fileList;
    std::string line;
    while (std::getline(fileListStream, line))
    {
        line.pop_back();

        ParsedFtpLine parsedLine = parseFtpListLine(line);

        std::cout << parsedLine.toStr();

        if (not parsedLine.isDotOrDotDotDir())
        {
            std::cout << line << std::endl;
            fileList.push_back(line);
        }
    }
    session.endList();
    return fileList;
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
    char c_strLine[line.size()];
    strcpy(c_strLine, line.c_str());
    ftpparse(&parsedLine, c_strLine, line.size());
    ParsedFtpLine parsedFtpLine(parsedLine);
    return parsedFtpLine;
}
