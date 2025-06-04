#ifndef __DATAENGINE_SRC_TOOL_TOOL_H__
#define __DATAENGINE_SRC_TOOL_TOOL_H__

#include <string>
#include <vector>
#include "Function/Bind.h"
#include "IDataEngine.h"

namespace engine {

#define ROOT_PATH std::string("DataEngine/")
#define TASK_CFG std::string("TaskCfg/")
#define DEVSTORE_CFG std::string("DevStore/")
#define TRANS_CFG std::string("TransStore/")
#define TEMPLATE_CFG std::string("Template/")

using logFunc = TFunction<void, const std::string&>;

static std::map<std::string, logFunc> sg_mapLogFunc;

enum PRINT_TYPE {
    SOURCE,
    FRAME
};

enum BUF_DIRCTION {
    SEND,
    RECV
};

bool openFilePath(const std::string &filePath, const TFunction<bool, const std::string &, const std::string &, FILE_TYPE > &func);
void splitStr(const std::string &symbol, const std::string &str, std::vector<std::string> &result);
void printBuf(const std::string &name, PRINT_TYPE type, const std::string &strSign, BUF_DIRCTION dirction, const std::string &frameBuf, bool bPrint);
void attachLogFunc(const std::string, const logFunc &func);
};

#endif /* __DATAENGINE_SRC_TOOL_TOOL_H__ */
