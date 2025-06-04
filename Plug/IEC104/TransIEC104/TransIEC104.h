#ifndef __DATAENGINEPLUG_IEC104_TRANSIEC104_TRANSIEC104_H__
#define __DATAENGINEPLUG_IEC104_TRANSIEC104_TRANSIEC104_H__

#include "DataEngine/IProtocol.h"
#include <mutex>
#include <set>
#include "Print/Print.h"
#include "Time/Time.h"

namespace engine {

//类型标示
enum CTRL_TYPE104                 //主站传输类型标志
{
	C_SC_NA_1	= 45	,	//	单点遥控
	C_DC_NA_1	= 46	,	//	双点遥控
	C_RC_NA_1	= 47	,	//	升降命令
	C_SE_NA_1 	= 48	,	//	设定规一化值命令
	C_SE_NB_1 	= 49	,	//	设定比例系数值命令
	C_SE_NC_1 	= 50	,	//	设定浮点数值命令
	C_BO_NA_1	= 51	,	//	32位比特串
	C_SC_TA_1 	= 58	,	//	带长时标单点遥控(CP56)
	C_DC_TA_1 	= 59	,	//	带长时标双点遥控(CP56)
	C_RC_TA_1	= 60	,	//	带长时标升降命令(CP56)
	C_SE_TA_1	= 61	,	//	带长时标设定规一化值命令(CP56)
	C_SE_TB_1 	= 62	,	//	带长时标设定比例系数值命令(CP56)
	C_SE_TC_1 	= 63	,	//	带长时标设定浮点数值命令(CP56)
	C_BO_TA_1 	= 64	,	//	带长时标32位比特串(CP56)
	C_IC_NA_1 	= 100	,	//	总召唤
	C_CI_NA_1 	= 101	,	//	电能脉冲召唤命令
	C_RD_NA_1 	= 102	,	//	读数据命令
	C_CS_NA_1 	= 103	,	//	时钟同步命令
	C_TS_NA_1 	= 104	,	//	测试命令
	C_RP_NA_1 	= 105	,	//	复位进程命令
	C_CD_NA_1 	= 106	,	//	延时获得命令
	C_TS_TA_1 	= 107	,	//	带长时标测试命令
	P_ME_NA_1 	= 110	,	//	规一化值的参数
	P_ME_NB_1 	= 111	,	//	比例系数的参数
	P_ME_NC_1 	= 112	,	//	浮点数值的参数
	P_AC_NA_1 	= 113	,	//	参数的激活
	P_SC_NA_1   = 114   ,   //  参数定值
	P_CV_NA_1   = 115   ,   //  参数修改
};

//监视方向传输类型标志
enum MONITOR_TYPE104
{
	M_SP_NA_1 	= 1	,	//单点遥信变位
	M_SP_TA_1 	= 2	,	//单点遥信变位短时标
	M_DP_NA_1 	= 3	,	//双点遥信变位
	M_DP_TA_1 	= 4	,	//双点遥信变位短时标
	M_ST_NA_1 	= 5	,	//步长位置信息
	M_ST_TA_1 	= 6	,	//带短时标的步长位置信息
	M_BO_NA_1 	= 7	,	//32位比特串
	M_BO_TA_1 	= 8	,	//带短时标的32位比特串
	M_ME_NA_1 	= 9	,	//带品质描述的规一化值
	M_ME_TA_1 	= 10 ,	//带短时标带品质描述的规一化值
	M_ME_NB_1 	= 11 ,  //带品质描述的比例系数
	M_ME_TB_1 	= 12 ,  //带短时标带品质描述的比例系数
	M_ME_NC_1 	= 13 ,  //短浮点数
	M_ME_TC_1 	= 14 ,  //带短时标的短浮点数
	M_IT_NA_1 	= 15 ,  //累计值
	M_IT_TA_1 	= 16 ,  //带短时标的累计值
	M_EP_TA_1 	= 17 ,  //带时标的继电保护或重合闸设备单个事件
	M_EP_TB_1 	= 18 ,  //带时标的继电保护装置成组启动事件
	M_EP_TC_1 	= 19 ,  //带时标的继电保护装置成组输出电路信息事件
	M_PS_NA_1 	= 20 ,  //具有状态变位检出的成组单点信息
	M_ME_ND_1 	= 21 ,  //不带品质描述的规一化值
	M_SP_TB_1 	= 30 ,  //带 CP56Time2a 时标的单点信息
	M_DP_TB_1 	= 31 ,  //带 CP56Time2a 时标的双点信息
	M_ST_TB_1 	= 32 ,  //带 CP56Time2a 时标的步位置信息
	M_BO_TB_1 	= 33 ,  //带 CP56Time2a 时标的 32 比特串
	M_ME_TD_1 	= 34 ,  //带 CP56Time2a 时标的测量值规一化值
	M_ME_TE_1 	= 35 ,  //带 CP56Time2a 时标的测量值标度化值
	M_ME_TF_1	= 36 ,  //带 CP56Time2a 时标的测量值短浮点数
	M_IT_TB_1	= 37 ,  //带 CP56Time2a 时标的累计量
	M_EP_TD_1	= 38 ,  //带 CP56Time2a 时标的继电保护设备事件
	M_EP_TE_1	= 39 ,  //带 CP56Time2a 时标的继电保护设备成组启动事件
	M_EP_TF_1	= 40 ,  //带 CP56Time2a 时标的继电保拌装置成组输出电路信息

	M_EI_NA_1 	= 70   //初始化结束

};

//传输原因
enum COT104               //传输原因
{
	PER_CRC_104 = 1	,     //周期循环per cyc
	BACK_104 = 2	,     //背景扫描back
	SPONT_104 = 3	,     //突发spont
	INIT_104 = 4	,     //初始化init
	REQ_104 = 5	,         //请求或被请求 req
	ACT_104 = 6	,         //激活act
	ACTCON_104 = 7	,     //激活确认actcon
	DEACT_104 = 8	,     //停止激活deact
	DEACTCON_104 = 9	, //停止激活确认deactcon
	ACTTERM_104 = 10	, //激活结束actterm
	RETREM_104 = 11	,     //远程命令引起的返送信息retrem
	RETLOC_104 = 12	,     //当地命令引起的返送信息retloc
	FILE_104 = 13	,     //文件传送file
	INTROGEN_104 = 20	, //响应总召唤introgen
	REQCOGCN = 37	,     //响应计数量总召唤reqcogcn
	REQCON1 = 38	,     //响应第 1组计数量召唤reqco1
	REQCON2 = 39	,     //响应第 2组计数量召唤reqco2
	REQCON3 = 40	,     //响应第 3组计数量召唤reqco3
	REQCON4 = 41	,     //响应第 4组计数量召唤reqco4

	UNDEFINE_TI = 44     ,   //未定义传输类型
	UNDEFINE_COT = 45     ,   //未定义传输原因
	UNDEFINE_COMM_ADDR = 46 ,     //未知的公共地址
    UNDEFINE_INFO_ADDR = 47     //未知的信息地址
};

struct APCI_104
{
	uint8_t head;      //0x68
	uint8_t apduLen;   //APDU的长度 最大长度为253
	uint8_t ctrlField1;   //控制域8位位组1
	uint8_t ctrlField2;   //控制域8位位组2
	uint8_t ctrlField3;   //控制域8位位组3
	uint8_t ctrlField4;   //控制域8位位组4
};

struct APDU_IDENTIFIER_104
{
	uint8_t type;                             //类型标示
	uint8_t sq;                             //可变结构限定词
	uint16_t cause;                //传输原因
	uint16_t addr;                 //站地址
};

struct APDU_CONTENT {
	APCI_104 apci;
	APDU_IDENTIFIER_104 apduIdent;
	uint8_t infoAddr[3];
	uint8_t data[0];
};

//U帧行为
#define TESTFR_CON    0x83
#define TESTFR_ACT    0x43
#define STOPDT_CON    0x23
#define STOPDT_ACT    0x13
#define STARTDT_CON   0x0B
#define STARTDT_ACT   0x07

/// apic(6) + asdu head(6) + 3 byte info addr
#define APHEAD_LEN 0x0f
#define SQ_MAX_NUM 0x7f

enum YX_TYPE {
	SINGLE_POINT, // 单点遥信
	DOUBLE_POINT, // 双点遥信
};

enum YC_TYPE {
	M_ME_NC_1_FLOAT,  //短浮点数遥测
	M_ME_ND_1_SHORT,  //无品质归一化
    M_ME_NA_1_SHORT,  //有品质归一化
	M_ME_NB_1_SHORT,  //标度化
};

class CTransIEC104 : public IProtocol
{
public:
    CTransIEC104();
    virtual ~CTransIEC104();
    SIMPLE_DEF_PROTOCOL(CTransIEC104)
public:
    virtual bool init(const Json::Value &cfgValue);
    virtual bool process(const std::vector<std::string> &vecDeviceName);
    virtual FIX_FRAME_VALUE fixFrame(const std::string &devName, const char *buf, int len, int &validLen);
    virtual std::string getProtocolName();

private:
    void newConnectedFunc(CONNECT_STATUS status, int fd);
    void handleMessage(int fd, const char *buf, int len);
	void increaseSendIdx();
	void sendFrame(const std::string &sendBuf);

private:
    void analyseFrame(const std::string &recvBuf);
    void analyseUFrame(const std::string &recvBuf);
    void analyseSFrame(const std::string &recvBuf);
    void sendUFrame(int val, std::string &response);
	void sendSFrame();

	void setAPCI(APCI_104 *apci, int len);
	void analyseIFrame(const std::string &recvBuf);
	void process_C_IC_NA_1(const std::string &frameBuf);
	void sendGIConfim(const std::string &frameBuf);
	void sendGIYx(const std::string &frameBuf, MONITOR_TYPE104 type, const std::string &strType);
	void sendGIYc(const std::string &frameBuf, MONITOR_TYPE104 type, const std::string &strType);
	void sendGIFinish(const std::string &frameBuf);

	void process_C_CS_NA_1(const std::string &frameBuf);
	void process_C_SC_NA_1(const std::string &frameBuf);

private:
	Json::Value m_cfgValue;
	Json::Value m_cfgPoint;
	int m_fd;
	uint16_t m_sendIdx;
	uint16_t m_recvIdx;
    std::mutex m_mutex;
    std::set<int> m_vecFd;
    std::string m_name;
    std::map<int, std::string> m_mapRecvBuf;
};

}

#endif /* __DATAENGINEPLUG_IEC104_TRANSIEC104_TRANSIEC104_H__ */
