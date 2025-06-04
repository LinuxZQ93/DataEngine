#ifndef __DATAENGINE_SRC_CONFIG_CONFIGPARSE_H__
#define __DATAENGINE_SRC_CONFIG_CONFIGPARSE_H__

#include <vector>
#include "SingTon/SingTon.h"
#include "Config/IConfigManager.h"
#include "Function/Bind.h"
#include "Tool/Tool.h"

namespace engine {

class CConfigParse {
SINGTON_DECLAR(CConfigParse)

public:
    bool parse();

    bool getTaskName(std::vector<std::string> &vecTask);
    bool getDeviceName(std::vector<std::string> &vecDev);
    bool getTransName(std::vector<std::string> &vecTrans);
    bool getDeviceCfg(const std::string &name, CfgType type, Json::Value &cfg);
    bool getTransCfg(const std::string &name, CfgType type, Json::Value &cfg);
    bool getCompositeCfg(Json::Value &cfg){ cfg = m_compositeCfg; return true; }
    /**
     * @brief 根据任务名称和类型获取配置信息
     *
     * @param name 任务名
     * @param type 配置类型
     * @param cfg 配置
     * @return 返回值
     */
    bool getTaskCfg(const std::string &name, CfgType type, Json::Value &cfg);
    /**
     * @brief 根据模板名称和类型获取配置信息
     *
     * @param name 模板名
     * @param type 配置类型
     * @param cfg 配置
     * @return 返回值
     */
    bool getTemplateCfg(const std::string &name, CfgType type, Json::Value &cfg);
    bool getTemplateParamCfg(const std::string &tempName, CfgType type, const std::string &paramName, Json::Value &cfg);
    /// 仅针对点表使用
    bool getTemplateParamCfg(const std::string &tempName, const std::string &dataType, const std::string &paramName, Json::Value &cfg);

    bool bPrintSource(const std::string &name);
    bool bPrintFrame(const std::string &name);

    bool transInfoToDevInfo(const std::string &transName, const std::string &strType, int transId, std::string &devName, int &pointId);

private:
    bool parseTask();
    bool parseDevice();
    bool parseTrans();
    bool parseComposite();
    bool parseTemplate();

private:
    base::IConfigManager *m_pConfig;
    std::map<std::string, std::map<CfgType, Json::Value>> m_mapTaskCfg;
    std::map<std::string, std::map<CfgType, Json::Value>> m_mapDeviceCfg;
    std::map<std::string, std::map<CfgType, Json::Value>*> m_mapDeviceNameCfg;
    Json::Value m_compositeCfg;
    std::map<std::string, Json::Value> m_mapCfgAttr;
    std::map<std::string, std::map<CfgType, Json::Value>> m_mapTransNameCfg;
    std::map<std::string, std::map<CfgType, Json::Value>> m_mapTemplateCfg;
};

}

#endif /* __DATAENGINE_SRC_CONFIG_CONFIGPARSE_H__ */
