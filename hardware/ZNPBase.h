#pragma once
#include "DomoticzHardware.h"
#include "serial/serial.h"
#include "ZNPParser.h"
#include "ZNPDefs.h"
#include "ZNPAf.h"
#include "ZNP_ZCL.h"
#include "ZNP_ZCLNode.h"
#include "../main/WebServer.h"

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/bind.hpp>

#define MAKE_UINT16(x)	((x)[0] | ((x)[1] << 8))
#define MAKE_UINT24(x)	((x)[0] | ((x)[1] << 8) | ((x)[2] << 16))
#define MAKE_UINT32(x)	((x)[0] | ((x)[1] << 8) | ((x)[2] << 16) | ((x)[3] << 24))

/* Default channel is Channel 11 - 0x0B               */
// Channels are defined in the following:
//         0      : 868 MHz     0x00000001
//         1 - 10 : 915 MHz     0x000007FE
//        11 - 26 : 2.4 GHz     0x07FFF800
//
//#define MAX_CHANNELS_868MHZ     0x00000001
//#define MAX_CHANNELS_915MHZ     0x000007FE
//#define MAX_CHANNELS_24GHZ      0x07FFF800
//#define DEFAULT_CHANLIST 0x04000000  // 26 - 0x1A
//#define DEFAULT_CHANLIST 0x02000000  // 25 - 0x19
//#define DEFAULT_CHANLIST 0x01000000  // 24 - 0x18
//#define DEFAULT_CHANLIST 0x00800000  // 23 - 0x17
//#define DEFAULT_CHANLIST 0x00400000  // 22 - 0x16
//#define DEFAULT_CHANLIST 0x00200000  // 21 - 0x15
//#define DEFAULT_CHANLIST 0x00100000  // 20 - 0x14  // Demo Channel
//#define DEFAULT_CHANLIST 0x00080000  // 19 - 0x13
//#define DEFAULT_CHANLIST 0x00040000  // 18 - 0x12
//#define DEFAULT_CHANLIST 0x00020000  // 17 - 0x11
//#define DEFAULT_CHANLIST 0x00010000  // 16 - 0x10
//#define DEFAULT_CHANLIST 0x00008000  // 15 - 0x0F
//#define DEFAULT_CHANLIST 0x00004000  // 14 - 0x0E
//#define DEFAULT_CHANLIST 0x00002000  // 13 - 0x0D
//#define DEFAULT_CHANLIST 0x00001000  // 12 - 0x0C
#define DEFAULT_CHANLIST 0x00000800  // 11 - 0x0B

#define JAMMER_DETECT_CONTINUOUS_EVENTS          150
#define JAMMER_DETECT_PERIOD_TIME                100  // In milliseconds
#define JAMMER_HIGH_NOISE_LEVEL                  60

#define NM_DEVICE_VERSION						 0

class ZNPBase;

class ZNPEvent
{
	boost::unordered_set<boost::shared_ptr<ZNPPacketCb_t> >	m_Cbs;

public:

	void operator () (const ZNPPacket_t *pkt) 
	{
		for (boost::shared_ptr<ZNPPacketCb_t> cb : m_Cbs)
		{
			(*cb)(pkt);
		}
	};
	
	ZNPEvent& operator+=(boost::shared_ptr<ZNPPacketCb_t> cb)
	{
		m_Cbs.insert(cb);
		return *this;
	}

	ZNPEvent& operator-=(boost::shared_ptr<ZNPPacketCb_t> cb)
	{
		m_Cbs.erase(cb);
		return *this;
	}
};

class ZNPCbRegAuto
{
	ZNPBase *m_znp;
	uint8_t m_subSys;
	uint8_t m_cmd;
	boost::shared_ptr<ZNPPacketCb_t> m_cb;

public:
	ZNPCbRegAuto(ZNPBase *znp, uint8_t subSys, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb);
	~ZNPCbRegAuto();
};

typedef struct
{
	devStates_t	state;
	uint16_t	shortAddr;
	uint16_t	panID;
	uint16_t	parentAddr;
	uint16_t	channel;
	uint8_t		extPanId[8];
	uint8_t		extparentAddr[8];
} ZNPNwkInfo_t;

typedef boost::shared_ptr<ZNPEvent>	ZNPEvent_t;
typedef boost::shared_ptr<boost::unordered_map<uint32_t, ZNPEvent_t> > ZNPSubsystemEvent_t;

class ZNPBase :
	public CDomoticzHardwareBase
{
	friend class ZNPCbRegAuto;
	friend class ZNP_ZCL;
	friend class ZNP_ZCLNode;
	friend class http::server::CWebServer;

	const uint16_t	m_cfgPanID = 0xFFFFu;
	const uint8_t	m_cfgType = 0;// DEVICE_BUILD_COORDINATOR;
	const uint32_t	m_cfgDefChList = DEFAULT_CHANLIST;
	const uint16_t  m_jammerContinuousEvents = JAMMER_DETECT_CONTINUOUS_EVENTS;
	const uint8_t   m_jammerHighNoiseLevel = JAMMER_HIGH_NOISE_LEVEL;
	const uint32_t  m_jammerDetectPeriodTime = JAMMER_DETECT_PERIOD_TIME;
	//uint8 maxSupportedEndpoints = MAX_SUPPORTED_ENDPOINTS;

	std::string							m_portName;
	boost::shared_ptr<serial::Serial>	m_port;
	boost::shared_ptr<boost::thread>	m_thread;
	bool								m_threadInitialized;
	bool								m_stoprequested;
	uint8_t								m_sendBuffer[MAX_ZNP_PACKET_LEN];
	uint8_t								m_cmdProcBuffer[MAX_ZNP_PACKET_LEN];
	ZNP_ZCL								m_zclReceiver = ZNP_ZCL(this);
	ZNPNwkInfo_t						m_nwkInfo;

	//sync requests support
	boost::interprocess::interprocess_condition	m_respWaitEvent;
	boost::interprocess::interprocess_mutex		m_respWaitLock;
	uint8_t										m_respBuffer[MAX_ZNP_PACKET_LEN];
	ZNPPacket_t									m_respPkt = ZNPPacket_t(m_respBuffer);
	boost::shared_ptr<ZNPPacketCb_t>			m_respCb;

	void onNewZNPPacket(const ZNPPacket_t *packet);

	boost::unordered_map<uint32_t, ZNPSubsystemEvent_t>	m_eventMap;
	boost::shared_ptr<ZNPParser>	m_parser;
	
	virtual bool StartHardware();
	virtual bool StopHardware();

	void Do_Work();

	void addEventCallback(uint8_t subsystem, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb);
	void delEventCallback(uint8_t subsystem, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb);

	void onSyncResponse(const ZNPPacket_t *packet);
	void onCbIncoming(const ZNPPacket_t *packet);
	void onEndDevAnnce(const ZNPPacket_t *packet);
	void onTCDevID(const ZNPPacket_t *packet);
	void onLeaveDevID(const ZNPPacket_t *packet);

	template<typename t>
	void storeLE(t val, uint8_t* buffer);

	bool znpInit();
	bool znpSetJammerParams(uint16_t contEvs, int8_t highNoiseLevel, uint32_t detectPeriod);
	bool znpEpRegister(const endPointDesc_t* epDesc);
	bool znpGetNwkInfo();
	bool znpPermitJoin(uint16_t address, uint8_t duration);
	bool znpGetAllNodes(uint8_t idx);
	bool znpGetNodeDescriptor(uint16_t address);

	static int32_t LQI2RSSI(uint8_t lqi)
	{
		static const int MIN_ED = -87;
		static const int MAX_ED = 10;
		return  (MIN_ED + lqi * (MAX_ED - MIN_ED) / 255);
	};

public:
	ZNPBase(const int ID, const std::string& devname);
	~ZNPBase();

	virtual bool WriteToHardware(const char *pdata, const unsigned char length);

	bool NVItemWrite(uint16_t item, const uint8_t *itemData, uint8_t itemSize);

	bool SendPacket(const ZNPPacket_t *packet);
	bool SendPacketWaitForRsp(uint8_t subSystem, uint8_t& cmd, uint8_t *data, uint8_t len, uint8_t& rspLen);
	bool SendPacketWaitForDefaultRsp(uint8_t subSystem, uint8_t cmd, uint8_t *data, uint8_t len);
	bool SendPacket(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len);
	bool SendPacketAReq(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len);
	bool SendPacketSReq(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len);
};

