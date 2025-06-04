#include "Tool.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include "Print/Print.h"
#include "Print/OSFuncDef.h"
#include "Time/Time.h"

namespace engine {

bool openFilePath(const std::string &filePath, const TFunction<bool, const std::string &, const std::string &, FILE_TYPE> &func)
{
    DIR *dir;
    struct dirent *ptr;

    if ((dir = opendir(filePath.c_str())) == NULL)
    {
        errorf("not exist config dir[%s]\n", filePath.c_str());
        return false;
    }
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
        {
            continue;
        }
        FILE_TYPE type;
        struct stat st;
        const std::string &path = filePath + "/" + ptr->d_name;
        int ret = stat(path.c_str(), &st);
        if (ret == -1)
        {
            warnf("detect d_name[%s] type failed\n", ptr->d_name);
            continue;
        }
        if ((st.st_mode & S_IFMT) == S_IFDIR)
        {
            type = F_DIR;
        }
        else if ((st.st_mode & S_IFMT) == S_IFREG)
        {
            type = F_REG;
        }
        else
        {
            type = F_UNKNOWN;
        }
        if (!func(filePath, ptr->d_name, type))
        {
            return false;
        }
    }
    closedir(dir);
    return true;
}

void splitStr(const std::string &symbol, const std::string &str, std::vector<std::string> &result)
{
    std::string::size_type pos = 0;
    std::string::size_type pos1 = str.find(symbol);
    bool bFind = false;
    while (pos1 != std::string::npos)
    {
        const std::string &tmp = str.substr(pos, pos1 - pos);
        if (!tmp.empty())
        {
            result.push_back(tmp);
        }
        pos = pos1 + 1;
        pos1 = str.find(symbol, pos1 + 1);
        bFind = true;
    }
    if (!bFind)
    {
        return;
    }
    if (pos != std::string::npos)
    {
        const std::string &tmp = str.substr(pos);
        if (!tmp.empty())
        {
            result.push_back(tmp);
        }
    }
}

void printBuf(const std::string &name, PRINT_TYPE type, const std::string &strSign, BUF_DIRCTION direction, const std::string &frameBuf, bool bPrint)
{
    std::string strType;
    std::string strDir;
    std::string strColor;
    if (type == SOURCE)
    {
        strType = "源码";
        strColor = COLOR_BLUE;
    }
    else
    {
        strType = "报文";
        strColor = COLOR_GREEN;
    }
    if (direction == SEND)
    {
        strDir = "发送";
    }
    else
    {
        strDir = "接收";
    }
    base::CTime::SysTime timeNow;
    base::CTime::getSysTime(timeNow);
    std::stringstream ss;
    ss.fill('0');
    ss << strType << ":[" << name << "]" << "[" << strDir << "][" << strSign << "]["
       << timeNow.year << "-" << std::setw(2) << timeNow.mon << "-" << std::setw(2)
       << timeNow.day << " " << std::setw(2) << timeNow.hour << ":" << std::setw(2)
       << timeNow.min << ":" << std::setw(2) << timeNow.sec << "." << timeNow.milliSec
       << "][" << "长度:" << frameBuf.size() << "]\n";
    for (uint32_t i = 0; i < frameBuf.size(); i++)
    {
        ss << std::hex << std::setw(2) << ((uint32_t)(frameBuf[i]) & 0xff) << " ";
    }
    ss << "\n";
    std::string rawBuffer = ss.str();
    if (sg_mapLogFunc.find(name) != sg_mapLogFunc.end())
    {
        sg_mapLogFunc[name](rawBuffer);
    }
    std::string buffer(strColor + rawBuffer + COLOR_NONE);
    if (bPrint)
    {
        fputs(buffer.c_str(), stdout);
    }
}

void attachLogFunc(const std::string name, const logFunc &func)
{
    sg_mapLogFunc[name] = func;
}

}
