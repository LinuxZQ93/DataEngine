#include "TransIEC104.h"

#include "Function/Bind.h"
#include "Time/Time.h"

namespace engine {

CTransIEC104::CTransIEC104()
    : m_sendIdx(0)
    , m_recvIdx(0)
{}

CTransIEC104::~CTransIEC104()
{}

bool CTransIEC104::init(const Json::Value &cfg)
{
    infof("CTransIEC104::init cfg: %s\n", cfg.toStyledString().c_str());
    const auto &cfgName = cfg["names"];
    m_name = cfgName[0u].asString();
    m_pDataEngine->attachConnectStatusFunc(m_name, base::function(&CTransIEC104::newConnectedFunc, this));
    m_pDataEngine->attachMsgAsyncFunc(m_name, base::function(&CTransIEC104::handleMessage, this));
    m_pDataEngine->getTranscfg(m_name, POINTTABLE, m_cfgPoint);
    m_pDataEngine->getTranscfg(m_name, PROTOCOL, m_cfgValue);
    m_cfgValue = m_cfgValue["param"];
    return true;
}

bool CTransIEC104::process(const std::vector<std::string> &vecTransName)
{
    m_mutex.lock();
    auto setFd = m_vecFd;
    m_mutex.unlock();
    const auto &name = vecTransName[0];
    return true;
}

FIX_FRAME_VALUE CTransIEC104::fixFrame(const std::string &devName, const char *buf, int len, int &validLen)
{
    if (buf[0] != 0x68)
    {
        return INVALID;
    }
    if (len < 2)
    {
        return INCOMPLETE;
    }
    int frameLen = int(buf[1]) + 2;
    if (frameLen > len)
    {
        return INCOMPLETE;
    }
    validLen = frameLen;
    return COMPLETE;
}

std::string CTransIEC104::getProtocolName()
{
    return "TransIEC104";
}

void CTransIEC104::newConnectedFunc(CONNECT_STATUS status, int fd)
{
    if (status == CONNECT)
    {
        infof("new fd[%d] connect\n", fd);
        std::lock_guard<std::mutex> Guard(m_mutex);
        m_vecFd.insert(fd);
    }
    else
    {
        infof("fd[%d] disconnect\n", fd);
        std::lock_guard<std::mutex> Guard(m_mutex);
        m_vecFd.erase(fd);
    }
}

void CTransIEC104::handleMessage(int fd, const char *buf, int len)
{
    int validLen = 0;
    m_mapRecvBuf[fd] += std::string(buf, len);
    std::string &recvBuf = m_mapRecvBuf[fd];
    m_fd = fd;
    do
    {
        FIX_FRAME_VALUE ret = fixFrame(m_name, recvBuf.c_str(), recvBuf.size(), validLen);
        if (ret == INCOMPLETE)
        {
            break;
        }
        else if (ret == INVALID)
        {
            recvBuf.clear();
            return;
        }
        std::string frameBuf = recvBuf.substr(0, validLen);
        recvBuf.erase(0, validLen);
        analyseFrame(frameBuf);
    } while (!recvBuf.empty());
}

void CTransIEC104::increaseSendIdx()
{
    m_sendIdx++;
    if (m_sendIdx > 32767)
    {
        m_sendIdx = 0;
    }
}

void CTransIEC104::sendFrame(const std::string &sendBuf)
{
    m_pDataEngine->sendFrame(m_fd, m_name, sendBuf);
    increaseSendIdx();
}

void CTransIEC104::analyseFrame(const std::string &frameBuf)
{
    APCI_104 *apci = (APCI_104*)frameBuf.c_str();
    if ((apci->ctrlField1 & 0x03) == 0x03)
    {
        analyseUFrame(frameBuf);
    }
    else if ((apci->ctrlField1 & 0x01) == 0x00)
    {
        m_recvIdx++;
        analyseIFrame(frameBuf);
    }
    else if (apci->ctrlField1 == 0x01)
    {
        analyseSFrame(frameBuf);
    }
}

void CTransIEC104::analyseUFrame(const std::string &frameBuf)
{
    APCI_104 *apci = (APCI_104*)frameBuf.c_str();
    std::string strType;
    std::string strType1;
    std::string response;
    switch (apci->ctrlField1)
    {
    case STARTDT_ACT:
        strType = "启动帧激活";
        strType1 = "启动帧应答";
        sendUFrame(STARTDT_CON, response);
        break;

    case TESTFR_ACT:
        strType = "测试帧激活";
        strType1 = "测试帧应答";
        sendUFrame(TESTFR_CON, response);
        break;

    case STOPDT_ACT:
        strType = "停止帧激活";
        strType1 = "停止帧应答";
        sendUFrame(STOPDT_CON, response);
        break;

    case TESTFR_CON:
        strType = "测试帧应答";
        break;

    default:
        break;
    }
    m_pDataEngine->printRecvFrame(m_name, strType, frameBuf);
    if (!strType1.empty())
    {
        m_pDataEngine->printSendFrame(m_name, strType1, response);
        m_pDataEngine->sendFrame(m_fd, m_name, response);
    }
}

void CTransIEC104::analyseSFrame(const std::string &frameBuf)
{
    m_pDataEngine->printRecvFrame(m_name, "S帧", frameBuf);
    /// 定时器的一些操作
}

void CTransIEC104::sendUFrame(int val, std::string &response)
{
    response.resize(6, '\0');
    response[0] = 0x68;
    response[1] = 0x04;
    response[2] = val;
    response[3] = 0x00;
    response[4] = 0x00;
    response[5] = 0x00;
}

void CTransIEC104::sendSFrame()
{
    std::string sendBuf(6, '\0');
    sendBuf[0] = 0x68;
    sendBuf[1] = 0x04;
    sendBuf[2] = 0x01;
    sendBuf[3] = 0x00;
    sendBuf[4] = m_recvIdx << 1;
    sendBuf[5] = m_recvIdx >> 8;
    m_pDataEngine->printSendFrame(m_name, "S帧", sendBuf);
    m_pDataEngine->sendFrame(m_fd, m_name, sendBuf);
}

void CTransIEC104::setAPCI(APCI_104 *apci, int len)
{
    apci->head = 0x68;
    apci->apduLen = len;
    apci->ctrlField1 = m_sendIdx << 1;
    apci->ctrlField2 = m_sendIdx >> 8;
    apci->ctrlField3 = m_recvIdx << 1;
    apci->ctrlField4 = m_recvIdx >> 8;
}

void CTransIEC104::analyseIFrame(const std::string &frameBuf)
{
    APDU_IDENTIFIER_104 *apdu = (APDU_IDENTIFIER_104 *)(&frameBuf[6]);
    switch (apdu->type)
    {
    case C_IC_NA_1:
        process_C_IC_NA_1(frameBuf);
        break;
    case C_CS_NA_1:
        process_C_CS_NA_1(frameBuf);
        break;
    case C_SC_NA_1:
        process_C_SC_NA_1(frameBuf);
        break;

    default:
        break;
    }
    sendSFrame();
}

void CTransIEC104::process_C_IC_NA_1(const std::string &frameBuf)
{
    m_pDataEngine->printRecvFrame(m_name, "总召唤", frameBuf);
    sendGIConfim(frameBuf);
    int yxType = m_cfgValue["yxType"].asInt();
    int ycType = m_cfgValue["ycType"].asInt();
    switch (yxType)
    {
    case SINGLE_POINT:
        sendGIYx(frameBuf, M_SP_NA_1, "总招单点遥信");
        break;
    case DOUBLE_POINT:
        sendGIYx(frameBuf, M_DP_NA_1, "总招双点遥信");
        break;

    default:
        break;
    }
    switch (ycType)
    {
    case M_ME_NC_1_FLOAT:
        sendGIYc(frameBuf, M_ME_NC_1, "总招浮点遥测");
        break;
    case M_ME_ND_1_SHORT:
        sendGIYc(frameBuf, M_ME_ND_1, "总招无品质归一化遥测");
        break;
    case M_ME_NA_1_SHORT:
        sendGIYc(frameBuf, M_ME_NA_1, "总招有品质归一化遥测");
        break;
    case M_ME_NB_1_SHORT:
        sendGIYc(frameBuf, M_ME_NB_1, "总招标度化遥测");
        break;
    default:
        break;
    }
    sendGIFinish(frameBuf);
}

void CTransIEC104::sendGIConfim(const std::string &frameBuf)
{
    std::string sendBuf(frameBuf);
    sendBuf[2] = m_sendIdx << 1;
    sendBuf[3] = m_sendIdx >> 8;
    sendBuf[4] = m_recvIdx << 1;
    sendBuf[5] = m_recvIdx >> 8;
    sendBuf[8] = ACTCON_104;
    sendBuf[9] = 0;
    m_pDataEngine->printSendFrame(m_name, "总召唤确认", sendBuf);
    sendFrame(sendBuf);
}

void CTransIEC104::sendGIYx(const std::string &frameBuf, MONITOR_TYPE104 type, const std::string &strType)
{
    std::string sendBuf(APHEAD_LEN + SQ_MAX_NUM, '\0');
    APDU_CONTENT *apduSend = (APDU_CONTENT *)&sendBuf[0];
    apduSend->apduIdent.type = type;
    apduSend->apduIdent.cause = INTROGEN_104;
    apduSend->apduIdent.addr = m_cfgValue["commAddr"].asInt();
    int signalNum = m_cfgPoint["param"]["signal"].size();
    if (signalNum == 0)
    {
        return;
    }
    std::vector<base::ValueWrap> vecValue;
    m_pDataEngine->getTransMultiValue(m_name, SIGNAL, 0, signalNum, vecValue);
    int n = signalNum / SQ_MAX_NUM;
    int k = 0;
    int yxStartAddr = m_cfgValue["yxStart"].asInt();
    do {
        int realNum = std::min(SQ_MAX_NUM, signalNum);
        apduSend->apduIdent.sq = 0x80 | realNum;
        apduSend->infoAddr[0] = yxStartAddr + k * SQ_MAX_NUM;
        apduSend->infoAddr[1] = (yxStartAddr + k * SQ_MAX_NUM) >> 8;
        apduSend->infoAddr[2] = (yxStartAddr + k * SQ_MAX_NUM) >> 16;
        for (int i = 0; i < realNum; i++)
        {
            apduSend->data[i] = vecValue[i + k * SQ_MAX_NUM].asUInt8();
        }
        setAPCI(&apduSend->apci, realNum + 13);
        sendBuf.resize(realNum + 15);
        m_pDataEngine->printSendFrame(m_name, strType, sendBuf);
        sendFrame(sendBuf);
        k++;
        signalNum = signalNum - SQ_MAX_NUM;
    } while (n-- > 0 && signalNum > 0);
}

void CTransIEC104::sendGIYc(const std::string &frameBuf, MONITOR_TYPE104 type, const std::string &strType)
{
    /// 5是4字节数据加1字节品质位
    std::string sendBuf(APHEAD_LEN + SQ_MAX_NUM * 5, '\0');
    APDU_CONTENT *apduSend = (APDU_CONTENT *)&sendBuf[0];
    apduSend->apduIdent.type = type;
    apduSend->apduIdent.cause = INTROGEN_104;
    apduSend->apduIdent.addr = m_cfgValue["commAddr"].asInt();
    int measureNum = m_cfgPoint["param"]["measure"].size();
    if (measureNum == 0)
    {
        return;
    }
    std::vector<base::ValueWrap> vecValue;
    m_pDataEngine->getTransMultiValue(m_name, SIGNAL, 0, measureNum, vecValue);
    int n = measureNum / SQ_MAX_NUM;
    int k = 0;
    int ycStartAddr = m_cfgValue["ycStart"].asInt();
    auto f = [&](int byteNum, int qualityNum)
    {
        do
        {
            int realNum = std::min(SQ_MAX_NUM, measureNum);
            apduSend->apduIdent.sq = 0x80 | realNum;
            apduSend->infoAddr[0] = ycStartAddr + k * SQ_MAX_NUM;
            apduSend->infoAddr[1] = (ycStartAddr + k * SQ_MAX_NUM) >> 8;
            apduSend->infoAddr[2] = (ycStartAddr + k * SQ_MAX_NUM) >> 16;
            int sum = byteNum + qualityNum;
            for (int i = 0; i < realNum; i += sum)
            {
                if (type == M_ME_NC_1)
                {
                    float value = vecValue[i + k * SQ_MAX_NUM].asFloat();
                    memcpy(&apduSend->data[i], &value, byteNum);
                }
                else if (type == M_ME_ND_1 || type == M_ME_NA_1 || type == M_ME_NB_1)
                {
                    short value = vecValue[i + k * SQ_MAX_NUM].asFloat();
                    memcpy(&apduSend->data[i], &value, byteNum);
                }
                if (qualityNum != 0)
                {
                    apduSend->data[i + sum] = 0;
                }
            }
            setAPCI(&apduSend->apci, realNum * sum + 13);
            sendBuf.resize(realNum * sum + 15);
            m_pDataEngine->printSendFrame(m_name, strType, sendBuf);
            sendFrame(sendBuf);
            k++;
            measureNum = measureNum - SQ_MAX_NUM;
        } while (n-- > 0 && measureNum > 0);
    };
    if (type == M_ME_NC_1)
    {
        f(4, 1);
    }
    else if (type == M_ME_ND_1)
    {
        f(2, 0);
    }
    else if (type == M_ME_NA_1 || type == M_ME_NB_1)
    {
        f(2, 1);
    }
}

void CTransIEC104::sendGIFinish(const std::string &frameBuf)
{
    std::string sendBuf(frameBuf);
    sendBuf[2] = m_sendIdx << 1;
    sendBuf[3] = m_sendIdx >> 8;
    sendBuf[4] = m_recvIdx << 1;
    sendBuf[5] = m_recvIdx >> 8;
    sendBuf[8] = ACTTERM_104;
    sendBuf[9] = 0;
    m_pDataEngine->printSendFrame(m_name, "总召唤结束", sendBuf);
    sendFrame(sendBuf);
}

void CTransIEC104::process_C_CS_NA_1(const std::string &frameBuf)
{
    std::string sendBuf(frameBuf);
    APDU_CONTENT *apduSend = (APDU_CONTENT *)&sendBuf[0];
    setAPCI(&apduSend->apci, apduSend->apci.apduLen);
    apduSend->apduIdent.cause = ACTCON_104;
    base::CTime::SysTime sTime;
    base::CTime::getSysTime(sTime);
    sTime.year = apduSend->data[6] + 2000;
    sTime.mon = apduSend->data[5];
    sTime.day = apduSend->data[4];
    sTime.hour = apduSend->data[3];
    sTime.min = apduSend->data[2];
    sTime.sec = (apduSend->data[1] << 8 + apduSend->data[0]) / 1000;
    base::CTime::setSysTime(sTime);
    m_pDataEngine->printSendFrame(m_name, "时钟同步", sendBuf);
    m_pDataEngine->sendFrame(m_fd, m_name, sendBuf);
}

void CTransIEC104::process_C_SC_NA_1(const std::string &frameBuf)
{
    std::string sendBuf(frameBuf);
    APDU_CONTENT *apduSend = (APDU_CONTENT *)&sendBuf[0];
    setAPCI(&apduSend->apci, apduSend->apci.apduLen);
}

extern "C"
{
    void initProtocol(const char *name)
    {
        CTransIEC104::CTransIEC104ProtocolFactory s_transIEC104(name);
    }
}

}
