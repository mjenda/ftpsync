#ifndef FTPMIRRORPOCO_H
#define FTPMIRRORPOCO_H

#include <Poco/Net/FTPClientSession.h>
#include <string>
#include <list>
#include <sstream>
#include <ftpparse.h>

struct FtpData;

using StringList = std::list<std::string>;

struct ParsedFtpLine
{
    ParsedFtpLine(struct ftpparse& parsedLine)
        : name(parsedLine.name, parsedLine.namelen),
          flagTryCwd(static_cast<bool>(parsedLine.flagtrycwd)),
          flagTryTetr(static_cast<bool>(parsedLine.flagtryretr)),
          fileSizeType(parsedLine.sizetype),
          fileSizeInOctets(parsedLine.size),
          modifiedTimeType(parsedLine.mtimetype),
          modifiedTime(parsedLine.mtime),
          idType(parsedLine.idtype),
          id(parsedLine.id, parsedLine.idlen)
    {}

    std::string name;
    bool        flagTryCwd;
    bool        flagTryTetr;
    int         fileSizeType;
    long        fileSizeInOctets;
    int         modifiedTimeType;
    time_t      modifiedTime;
    int         idType;
    std::string id;

    bool isDir()
    {
        return flagTryCwd;
    }

    bool isFile()
    {
        return flagTryTetr;
    }

    bool isDotOrDotDotDir()
    {
        if (name[0] == '.')
        {
            if (name.size() == 1)
            {
                return true;
            }
            else if (name.size() == 2 && name[1] == '.')
            {
                return true;
            }
        }
        return false;
    }

    std::string toStr() const
    {
        //std::string ret;
        std::ostringstream ret;
        ret << "FILENAME     = " << name             << "\n";
        ret << "ISDIR        = " << flagTryCwd       << "\n";
        ret << "ISFILE       = " << flagTryTetr      << "\n";
        ret << "FILESIZETYPE = " << fileSizeType     << "\n";
        ret << "FILESIZE     = " << fileSizeInOctets << "\n";
        ret << "MODTIMETYPE  = " << modifiedTimeType << "\n";
        ret << "MODTIME      = " << modifiedTime     << "\n";
        ret << "IDTYPE       = " << idType           << "\n";
        ret << "ID           = " << id               << "\n";
        return ret.str();
    }
};

class FtpMirrorPoco
{
public:
    FtpMirrorPoco(const FtpData &ftpData);
    bool getDictionary();
    StringList getFileListFromDirectory(const std::string& dirPath);

private:
    bool isDotOrDotDotDirectory(const std::string& file);
    ParsedFtpLine parseFtpListLine(const std::string& line);

    Poco::Net::FTPClientSession session;
};

#endif // FTPMIRRORPOCO_H
