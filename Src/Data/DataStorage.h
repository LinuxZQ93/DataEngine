#ifndef __DATAENGINE_SRC_DATA_DATASTORAGE_H__
#define __DATAENGINE_SRC_DATA_DATASTORAGE_H__

#include <map>
#include <vector>
#include <string>
#include <mutex>
#include "Config/ConfigParse.h"
#include "SingTon/SingTon.h"
#include "Data/ValueWrap.h"

namespace engine {

class CDataStorage {
    SINGTON_DECLAR(CDataStorage)

public:
    bool init();
    bool updateValue(const std::string &name, DATA_TYPE type, int pointId, const base::ValueWrap &data);
    bool updateMultiValue(const std::string &name, DATA_TYPE type, int pointId, const std::vector<base::ValueWrap> &vecData);
    bool updateValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const base::ValueWrap &data);
    bool updateMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, const std::vector<base::ValueWrap> &vecData);
    bool getValue(const std::string &name, DATA_TYPE type, int pointId, base::ValueWrap &data);
    bool getMultiValue(const std::string &name, DATA_TYPE type, int pointId, int num, std::vector<base::ValueWrap> &vecDataNew);
    bool getValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, base::ValueWrap &data);
    bool getMultiValue(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition, int num, std::vector<base::ValueWrap> &vecDataNew);
    bool getTransValue(const std::string &name, DATA_TYPE type, int transId, base::ValueWrap &data);
    bool getTransMultiValue(const std::string &name, DATA_TYPE type, int transId, int num, std::vector<base::ValueWrap> &data);
    bool getExprValue(DATA_TYPE type, const std::string &expr, base::ValueWrap &data);

private:
    void initData(const std::string &devName, const Json::Value &cfgPoint);
    int getIndex(const std::string &name, DATA_TYPE type, const std::pair<std::string, base::ValueWrap> &condition);
    double transValueToDouble(const base::ValueWrap &data);
    bool transValueToString(DATA_TYPE type, const base::ValueWrap &data, std::string &strData);
    bool getCompositeValue(DATA_TYPE type, int pointId, base::ValueWrap &data);

private:
    std::map<DATA_TYPE, std::string> m_mapType;
    std::mutex m_mutex;
    std::map<std::string, std::map<DATA_TYPE, std::vector<base::ValueWrap>>> m_mapValue;
};

}

#endif /* __DATAENGINE_SRC_DATA_DATASTORAGE_H__ */
