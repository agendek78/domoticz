#include "stdafx.h"
#include "ZNPBase.h"
#include "ZNPDefs.h"
#include "ZNPAf.h"
#include "ZNP_ZCL.h"
#include "../main/Logger.h"
#include "../main/WebServer.h"
#include "../main/mainworker.h"
#include "../json/json.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 
#include <boost/msm/common.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/format.hpp>
#include <algorithm>

/*********************************************************************
* SIMPLE DESCRIPTOR
*/
static const cId_t nmInClusterList[] =
{
	ZCL_CLUSTER_ID_GEN_BASIC,
	0
};
#define NWKMGR_INCLUSTERS ((sizeof(nmInClusterList) / sizeof(nmInClusterList[0])) - 1)

static const cId_t nmOutClusterList[] =
{
	ZCL_CLUSTER_ID_GEN_ON_OFF,
	0
};
#define NWKMGR_OUTCLUSTERS ((sizeof(nmOutClusterList) / sizeof(nmOutClusterList[0])) - 1)

static SimpleDescriptionFormat_t nmSimpleDesc =
{
	NM_EP,                         //  int    Endpoint;
	ZCL_HA_PROFILE_ID,             //  uint16 AppProfId[2];
	ZCL_HA_DEVICEID_COMBINED_INETRFACE, //  uint16 AppDeviceId[2];
	NM_DEVICE_VERSION,             //  int    AppDevVer:4;
	NWKMGR_INCLUSTERS,             //  uint8  AppNumInClusters;
	(cId_t *)nmInClusterList,      //  uint8 *pAppInClusterList;
	NWKMGR_OUTCLUSTERS,            //  uint8  AppNumInClusters;
	(cId_t *)nmOutClusterList      //  uint8 *pAppInClusterList;
};

static const endPointDesc_t	epDescr =
{
	&nmSimpleDesc, noLatencyReqs
};

using namespace serial;
using namespace std;
using namespace boost::interprocess;
using namespace boost::endian;

ZNPCbRegAuto::ZNPCbRegAuto(ZNPBase *znp, uint8_t subSys, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb) :
	m_znp(znp), m_subSys(subSys), m_cmd(cmd), m_cb(cb)
{
	m_znp->addEventCallback(m_subSys, m_cmd, m_cb);
}

ZNPCbRegAuto::~ZNPCbRegAuto()
{
	m_znp->delEventCallback(m_subSys, m_cmd, m_cb);
}

template<typename t>
void ZNPBase::storeLE(t val, uint8_t* buffer)
{
	t tmp = native_to_little(val);
	memcpy(buffer, &tmp, sizeof(t));
}

template<typename t>
t readLE(const uint8_t *buffer)
{
	t ret;
	memcpy(&ret, buffer, sizeof(t));
	return little_to_native(ret);
}

ZNPBase::ZNPBase(const int ID, const std::string& devname) :
	m_portName(devname), m_port(NULL)
{
	//_log.setLogVerboseLevel(VBL_TRACE);
	m_HwdID = ID;
	m_parser = boost::shared_ptr<ZNPParser>(new ZNPParser(boost::bind(&ZNPBase::onNewZNPPacket, this, _1)));
	m_respCb = boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNPBase::onSyncResponse, this, _1)));
}

ZNPBase::~ZNPBase()
{
	m_stoprequested = true;
	if (m_thread != NULL)
	{
		m_thread->join();
	}
}

void ZNPBase::onNewZNPPacket(const ZNPPacket_t *packet)
{
	_log.Log(_eLogLevel::LOG_STATUS, "%s new packet subsystem 0x%02x, cmd 0x%02x", __FUNCTION__, packet->cmdSubSystem, packet->cmdID);

	ZNPSubsystemEvent_t	subsys = m_eventMap[packet->cmdSubSystem & RPC_SUBSYSTEM_MASK];
	if (subsys != NULL)
	{
		ZNPEvent_t cmdEv = (*subsys)[packet->cmdID];
		if (cmdEv != NULL)
		{
			(*cmdEv)(packet);
		}
	}
}

void ZNPBase::onEndDevAnnce(const ZNPPacket_t *packet)
{
  uint16_t srcAddr = readLE<uint16_t>(packet->data);
  uint16_t nwkAddr = readLE<uint16_t>(&packet->data[2]);

  string caps = packet->data[12] & 0x01 ? "router" : "end dev";

  if (packet->data[12] & 0x02)
    caps += ", mains powered";

  if (packet->data[12] & 0x04)
    caps += ", rx on when idle";

  if (packet->data[12] & 0x40)
    caps += ", security caps";

  _log.Log(_eLogLevel::LOG_STATUS, "END device announce src 0x%04x, nwkAdr 0x%04x, caps %s",
           srcAddr, nwkAddr, caps.c_str());
}

void ZNPBase::onTCDevID(const ZNPPacket_t *packet)
{
  uint16_t srcAddr = readLE<uint16_t>(packet->data);
  uint64_t ieeeAddr = readLE<uint64_t>(&packet->data[2]);
  uint16_t nwkAddr = readLE<uint16_t>(&packet->data[10]);

  _log.Log(_eLogLevel::LOG_STATUS, "TC device IND src 0x%04x, nwkAdr 0x%04x, IEEE %lx",
           srcAddr, nwkAddr, ieeeAddr);
}

void ZNPBase::onLeaveDevID(const ZNPPacket_t *packet)
{
  uint16_t srcAddr = readLE<uint16_t>(packet->data);
  uint64_t ieeeAddr = readLE<uint64_t>(&packet->data[2]);

  string flags = packet->data[10] == 0 ? "indication" : "request";

  if (packet->data[11] != 0)
    flags += ", remove children";

  if (packet->data[12] != 0)
    flags += ", rejoin";

  _log.Log(_eLogLevel::LOG_STATUS, "TC device IND src 0x%04x, IEEE %lx, flags: %s",
           srcAddr, ieeeAddr, flags.c_str());
}

void ZNPBase::onCbIncoming(const ZNPPacket_t *packet)
{
	uint16_t srcAddr = readLE<uint16_t>(packet->data);
	uint16_t clusterID = readLE<uint16_t>(&packet->data[3]);
	const uint8_t *data = &packet->data[9];
	int16_t data_len = packet->length - 9;

	switch (clusterID)
	{
	case NWK_addr_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster NWK_addr_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case IEEE_addr_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster IEEE_addr_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Node_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Node_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Power_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Power_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Simple_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Simple_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Active_EP_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Active_EP_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Match_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Match_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case NWK_addr_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster NWK_addr_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case IEEE_addr_rsp: 
		_log.Log(_eLogLevel::LOG_STATUS, "cluster IEEE_addr_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); 
		_log.Log(_eLogLevel::LOG_STATUS, "status 0x%02x, num devices %d", data[0], data[11]);
		break;
	case Node_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Node_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Power_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Power_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Simple_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Simple_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Active_EP_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Active_EP_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Match_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Match_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Complex_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Complex_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case User_Desc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster User_Desc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Discovery_Cache_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Discovery_Cache_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Device_annce: _log.Log(_eLogLevel::LOG_STATUS, "cluster Device_annce, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case User_Desc_set: _log.Log(_eLogLevel::LOG_STATUS, "cluster User_Desc_set, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Server_Discovery_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Server_Discovery_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case End_Device_Timeout_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster End_Device_Timeout_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Complex_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Complex_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case User_Desc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster User_Desc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Discovery_Cache_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Discovery_Cache_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case User_Desc_conf: _log.Log(_eLogLevel::LOG_STATUS, "cluster User_Desc_conf, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Server_Discovery_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Server_Discovery_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case End_Device_Timeout_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster End_Device_Timeout_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case End_Device_Bind_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster End_Device_Bind_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Bind_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Bind_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Unbind_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Unbind_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Bind_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Bind_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case End_Device_Bind_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster End_Device_Bind_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Unbind_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Unbind_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_NWK_Disc_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_NWK_Disc_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Lqi_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Lqi_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Rtg_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Rtg_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Bind_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Bind_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Leave_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Leave_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Direct_Join_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Direct_Join_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Permit_Join_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Permit_Join_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_NWK_Update_req: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_NWK_Update_req, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_NWK_Disc_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_NWK_Disc_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Lqi_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Lqi_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Rtg_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Rtg_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Bind_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Bind_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Leave_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Leave_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Direct_Join_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Direct_Join_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_Permit_Join_rsp: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_Permit_Join_rsp, dev 0x%04x, data len = %d", srcAddr, data_len); break;
	case Mgmt_NWK_Update_notify: _log.Log(_eLogLevel::LOG_STATUS, "cluster Mgmt_NWK_Update_notify, dev 0x%04x, data len = %d", srcAddr, data_len); break;


	default:
		_log.Log(_eLogLevel::LOG_STATUS, "%s: src 0x%04x, cluster 0x%04x, data len = %d", __FUNCTION__,
			srcAddr, clusterID, data_len);
	}

	ZNP_ZCL::dumpBuffer(data, data_len);
}

void OnVerionRsp(const ZNPPacket_t *packet)
{
	_log.Log(_eLogLevel::LOG_STATUS, "%s: Transport v%02x, ProductID 0x%02x, Version %d.%d.%d!", __FUNCTION__,
				packet->data[0], packet->data[1], packet->data[2], packet->data[3], packet->data[4]);
}

bool ZNPBase::znpGetAllNodes(uint8_t idx)
{
	bool ret;
	uint8_t req[4];

	storeLE(m_nwkInfo.shortAddr, req);
	req[2] = 0x01;
	req[3] = idx;
	ret = SendPacketSReq(RPC_SYS_ZDO, MT_ZDO_IEEE_ADDR_REQ, req, sizeof(req));

	return ret;
}

bool ZNPBase::znpGetNodeDescriptor(uint16_t address)
{
	bool ret;
	uint8_t req[4];

	storeLE(m_nwkInfo.shortAddr, req);
	storeLE(address, &req[2]);
	ret = SendPacketWaitForDefaultRsp(RPC_SYS_ZDO, MT_ZDO_NODE_DESC_REQ, req, sizeof(req));

	return ret;
}

bool ZNPBase::znpInit()
{
	bool ret = false;
	uint8_t reqData[4] = { 0xFF, 0xFF, 0, 0 };

	ret = SendPacketWaitForDefaultRsp(RPC_SYS_ZDO, MT_ZDO_MSG_CB_REGISTER, reqData, 2);
	if (ret != true)
	{
	  _log.Log(_eLogLevel::LOG_ERROR, "%s: unable to register ZDO message callback!", __FUNCTION__);
	}
	
	storeLE(m_cfgDefChList, reqData);
	ret = NVItemWrite(ZCD_NV_CHANLIST, reqData, sizeof(m_cfgDefChList));
	storeLE(m_cfgPanID, reqData);
	ret = NVItemWrite(ZCD_NV_PANID, reqData, sizeof(m_cfgPanID));
	ret = NVItemWrite(ZCD_NV_LOGICAL_TYPE, &m_cfgType, sizeof(m_cfgType));
	ret = znpSetJammerParams(m_jammerContinuousEvents, (m_jammerHighNoiseLevel * -1), m_jammerDetectPeriodTime);
	ret = SendPacketAReq(RPC_SYS_ZDO, MT_ZDO_STARTUP_FROM_APP, NULL, 0);

	ret = znpEpRegister(&epDescr);

	if (znpGetNwkInfo() == true)
	{
		_log.Log(_eLogLevel::LOG_STATUS, "Nwk addr 0x%04x", m_nwkInfo.shortAddr);
		_log.Log(_eLogLevel::LOG_STATUS, "Nwk PANID 0x%04x", m_nwkInfo.panID);
		_log.Log(_eLogLevel::LOG_STATUS, "Parent addr 0x%04x", m_nwkInfo.parentAddr);

		znpPermitJoin(0x0000, 0xFF);
		reqData[0] = 0x81;
		ret = SendPacketWaitForDefaultRsp(RPC_SYS_ZDO, MT_ZDO_EXT_SET_PARAMS, reqData, 1);

		//znpGetAllNodes(0);
		znpGetNodeDescriptor(0xb1ca);
	}
	else
	{
		_log.Log(_eLogLevel::LOG_ERROR, "%s: unable to get network configuration!", __FUNCTION__);
	}

	return ret;
}

bool ZNPBase::znpPermitJoin(uint16_t address, uint8_t duration)
{
	bool ret = false;
	uint8_t data[5];

	data[0] = 0x02;
	data[3] = duration;
	data[4] = 0;

	if ((address == 0xFFFF) || (address == 0xFFFD)
		|| (address == 0xFFFC))
	{
		storeLE(m_nwkInfo.shortAddr, &data[1]);
		ret = SendPacketWaitForDefaultRsp(RPC_SYS_ZDO, MT_ZDO_MGMT_PERMIT_JOIN_REQ, data, sizeof(data));
	}

	storeLE(address, &data[1]);
	
	ret &= SendPacketWaitForDefaultRsp(RPC_SYS_ZDO, MT_ZDO_MGMT_PERMIT_JOIN_REQ, data, sizeof(data));

	return ret;
}

bool ZNPBase::znpSetJammerParams(uint16_t contEvs, int8_t highNoiseLevel, uint32_t detectPeriod)
{
	storeLE(contEvs, m_cmdProcBuffer);
	m_cmdProcBuffer[2] = (uint8_t)highNoiseLevel;
	storeLE(detectPeriod, &m_cmdProcBuffer[3]);

	return SendPacketWaitForDefaultRsp(RPC_SYS_SYS, MT_SYS_JAMMER_PARAMETERS, m_cmdProcBuffer, sizeof(uint16_t) + 1 + sizeof(uint32_t));
}

bool ZNPBase::znpEpRegister(const endPointDesc_t* epDesc)
{
	const uint8_t len = ((epDesc->simpleDesc->AppNumInClusters) * 2)
		+ ((epDesc->simpleDesc->AppNumOutClusters) * 2) + 9;

	m_cmdProcBuffer[0] = epDesc->simpleDesc->EndPoint;
	storeLE(epDesc->simpleDesc->AppProfId, &m_cmdProcBuffer[1]);
	storeLE(epDesc->simpleDesc->AppDeviceId, &m_cmdProcBuffer[3]);
	m_cmdProcBuffer[5] = epDesc->simpleDesc->AppDevVer;
	m_cmdProcBuffer[6] = (uint8_t)epDesc->latencyReq;
	m_cmdProcBuffer[7] = epDesc->simpleDesc->AppNumInClusters;
	for (uint8_t idx = 0; idx < epDesc->simpleDesc->AppNumInClusters; idx++)
	{
		storeLE((uint16_t)epDesc->simpleDesc->pAppInClusterList[idx], &m_cmdProcBuffer[8 + idx * sizeof(uint16_t)]);
	}
	m_cmdProcBuffer[8 + epDesc->simpleDesc->AppNumInClusters * sizeof(uint16_t)] = epDesc->simpleDesc->AppNumOutClusters;
	for (uint16_t idx = 0; idx < epDesc->simpleDesc->AppNumOutClusters; idx++)
	{
		storeLE((uint16_t)epDesc->simpleDesc->pAppOutClusterList[idx], &m_cmdProcBuffer[8 + epDesc->simpleDesc->AppNumInClusters * sizeof(uint16_t) + idx * sizeof(uint16_t) + 1]);
	}

	return SendPacketWaitForDefaultRsp(RPC_SYS_AF, MT_AF_REGISTER, m_cmdProcBuffer, len);
}

bool ZNPBase::znpGetNwkInfo()
{
	uint8_t cmd = MT_ZDO_EXT_NWK_INFO;
	uint8_t buffer[32] = {0};
	uint8_t rspLen = sizeof(buffer);
	
	bool ret = SendPacketWaitForRsp(RPC_SYS_ZDO, cmd, buffer, 0, rspLen);
	if (ret == true)
	{
		m_nwkInfo.shortAddr = readLE<uint16_t>((const uint8_t *) buffer);
		m_nwkInfo.state = (devStates_t) buffer[2];
		m_nwkInfo.panID = readLE<uint16_t>((const uint8_t *) &buffer[3]);
		m_nwkInfo.parentAddr = readLE<uint16_t>((const uint8_t *)&buffer[5]);
		memcpy(m_nwkInfo.extPanId, &buffer[7], sizeof(m_nwkInfo.extPanId));
		memcpy(m_nwkInfo.extparentAddr, &buffer[7 + 8], sizeof(m_nwkInfo.extparentAddr));
		m_nwkInfo.channel = buffer[7 + 8 + 8];
	}

	return ret;
}

bool ZNPBase::NVItemWrite(uint16_t item, const uint8_t *itemData, uint8_t itemSize)
{
	storeLE(item, m_cmdProcBuffer);
	m_cmdProcBuffer[2] = 0;
	m_cmdProcBuffer[3] = itemSize;
	memcpy(&m_cmdProcBuffer[4], itemData, itemSize);
	return SendPacketWaitForDefaultRsp(RPC_SYS_SYS, MT_SYS_OSAL_NV_WRITE, m_cmdProcBuffer, itemSize + 4);
}

void ZNPBase::Do_Work()
{
	uint8_t	data[MAX_ZNP_PACKET_LEN];
	uint32_t hbCntr = 0;

	addEventCallback(RPC_SYS_SYS, MT_SYS_VERSION, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(&OnVerionRsp)));
	addEventCallback(RPC_SYS_AF, MT_AF_INCOMING_MSG, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNP_ZCL::OnNewPacket, &m_zclReceiver, _1))));
	addEventCallback(RPC_SYS_ZDO, MT_ZDO_MSG_CB_INCOMING, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNPBase::onCbIncoming, this, _1))));
	addEventCallback(RPC_SYS_ZDO, MT_ZDO_END_DEVICE_ANNCE_IND, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNPBase::onEndDevAnnce, this, _1))));
	addEventCallback(RPC_SYS_ZDO, MT_ZDO_TC_DEVICE_IND, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNPBase::onTCDevID, this, _1))));
	addEventCallback(RPC_SYS_ZDO, MT_ZDO_LEAVE_IND, boost::shared_ptr<ZNPPacketCb_t>(new ZNPPacketCb_t(boost::bind(&ZNPBase::onLeaveDevID, this, _1))));

	SendPacketSReq(RPC_SYS_SYS, MT_SYS_VERSION, NULL, 0);
	boost::this_thread::sleep(boost::posix_time::milliseconds(500));

	sOnConnected(this);

	_log.Log(_eLogLevel::LOG_STATUS, "%s ZNP started!", __FUNCTION__);

	m_stoprequested = false;
	m_threadInitialized = true;
	while (m_stoprequested == false)
	{
		try
		{
			size_t len = m_port->available();
			if (len > 0)
			{
				if (len > MAX_ZNP_PACKET_LEN)
				{
					len = MAX_ZNP_PACKET_LEN;
				}

				m_port->read(data, len);
				m_parser->ZNPDataPush(data, len);
			}
		}
		catch (...)
		{

		}

		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		
		//if ((hbCntr++ % 10) == 0)
		//{
		//	SetHeartbeatReceived();
		//}
	}
}

bool ZNPBase::StartHardware()
{
	_log.Log(_eLogLevel::LOG_TRACE, "%s", __FUNCTION__);

	try
	{
		m_port = boost::shared_ptr<Serial>(new Serial(m_portName, 115200));//, Timeout(), eightbits, parity_none, stopbits_one, flowcontrol_hardware));
		if (m_port->isOpen() == true)
		{
		  Timeout tm = Timeout(10, 100, 0, 100, 1);

		  m_port->setTimeout(tm);
			m_threadInitialized = false;
			m_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&ZNPBase::Do_Work, this)));
			
			if (m_thread != NULL)
			{
				while (m_threadInitialized == false)
				{
					boost::this_thread::sleep(boost::posix_time::milliseconds(10));
				}

				znpInit();
				StartHeartbeatThread();
			}

			return (m_thread != NULL);
		}
	}	
	catch (...)
	{
		_log.Log(_eLogLevel::LOG_ERROR, "Unable to start ZNP hardware!");
	}

	return false;
}

bool ZNPBase::StopHardware()
{
	_log.Log(_eLogLevel::LOG_TRACE, "%s", __FUNCTION__);

	StopHeartbeatThread();

	m_stoprequested = true;
	if (m_thread != NULL)
	{
		m_thread->join();
	}

	if (m_port != NULL)
	{
		m_port->close();
	}

	return true;
}

bool ZNPBase::WriteToHardware(const char *pdata, const unsigned char length)
{
	_log.Log(_eLogLevel::LOG_TRACE, "%s", __FUNCTION__);
	return true;
}

bool ZNPBase::SendPacket(const ZNPPacket_t *packet)
{
	if (packet != NULL)
	{
		return SendPacket(packet->cmdSubSystem, packet->cmdSubSystem, packet->data, packet->length);
	}
	return false;
}

bool ZNPBase::SendPacketAReq(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len)
{
	return SendPacket((subSystem & RPC_SUBSYSTEM_MASK) | RPC_CMD_AREQ, cmd, data, len);
}

bool ZNPBase::SendPacketSReq(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len)
{
	return SendPacket((subSystem & RPC_SUBSYSTEM_MASK) | RPC_CMD_SREQ, cmd, data, len);
}


void ZNPBase::onSyncResponse(const ZNPPacket_t *packet)
{
	//_log.Log(_eLogLevel::LOG_STATUS, "%s: got response 0x%02x", __FUNCTION__, packet->cmdID);
	scoped_lock<interprocess_mutex>		lock(m_respWaitLock);

	if (packet->length > 0)
	{
		memcpy(m_respBuffer, packet->data, packet->length);
	}
	memcpy(&m_respPkt, packet, sizeof(ZNPPacket_t));
	m_respPkt.data = reinterpret_cast<const uint8_t *>(m_respBuffer);

	m_respWaitEvent.notify_all();
}

bool ZNPBase::SendPacketWaitForDefaultRsp(uint8_t subSystem, uint8_t cmd, uint8_t *data, uint8_t len)
{
	uint8_t respLen = len;
	bool ret = SendPacketWaitForRsp(subSystem, cmd, data, len, respLen);

	if (ret == true && respLen > 0)
	{
		if (data[0] != RPC_SUCCESS)
		{
			_log.Log(_eLogLevel::LOG_ERROR, "%s: ZNP responded with error code 0x%02x", __FUNCTION__, data[0]);
			ret = false;
		}
	}

	return ret;
}

bool ZNPBase::SendPacketWaitForRsp(uint8_t subSystem, uint8_t& cmd, uint8_t *data, uint8_t len, uint8_t& rspLen)
{
	bool ret;
	ZNPCbRegAuto cb(this, subSystem, cmd, m_respCb);
	
	_log.Log(_eLogLevel::LOG_STATUS, "%s: sending cmd 0x%02x", __FUNCTION__, cmd);
	ret = SendPacketSReq(subSystem, cmd, data, len);
	if (ret == true)
	{
		boost::posix_time::ptime			timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
		scoped_lock<interprocess_mutex>		lock(m_respWaitLock);

		ret = m_respWaitEvent.timed_wait(lock, timeout);
		if (ret == true)
		{
			cmd = m_respPkt.cmdID;
#if defined(__GNUC__)
      memcpy(data, m_respPkt.data, m_respPkt.length);
#else
			memcpy_s(data, rspLen, m_respPkt.data, m_respPkt.length);
#endif
			rspLen = min(m_respPkt.length, rspLen);
		}
		else
		{
		  _log.Log(_eLogLevel::LOG_STATUS, "%s: timeout waiting for cmd %02x", __FUNCTION__, cmd);
		}
	}
	else
	{
	  _log.Log(_eLogLevel::LOG_ERROR, "%s: error sending cmd %02x", __FUNCTION__, cmd);
	}

	return ret;
}

bool ZNPBase::SendPacket(uint8_t subSystem, uint8_t cmd, const uint8_t *data, uint8_t len)
{
	if (m_port->isOpen() == true)
	{
		m_sendBuffer[0] = RPC_UART_SOF;
		m_sendBuffer[1] = len;
		m_sendBuffer[2] = subSystem;
		m_sendBuffer[3] = cmd;
		if (len > 0)
		{
			if (data == NULL)
			{
				return false;
			}
			BOOST_ASSERT(data != NULL);

			memcpy(&m_sendBuffer[GFF_HEADER_LEN + UART_FRAME_HEADER_LEN], data, len);
		}

		m_sendBuffer[GFF_HEADER_LEN + UART_FRAME_HEADER_LEN + len] = ZNPParser::calcFCS(&m_sendBuffer[ZNP_FRAME_LEN_POS], GFF_HEADER_LEN + len);

		size_t bytesSent = m_port->write(m_sendBuffer, len + ZNP_FRAME_HEAD_FT_LEN);
		if (bytesSent > 0)
		{
			return true;
		}
		else
		{
		  _log.Log(_eLogLevel::LOG_STATUS, "%s: UART write error. Bytes sent %lu!", __FUNCTION__, bytesSent);
		}
	}
	else
	{
	  _log.Log(_eLogLevel::LOG_STATUS, "%s: UART port not open!", __FUNCTION__);
	}

	return false;
}

void ZNPBase::addEventCallback(uint8_t subsystem, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb)
{
	ZNPSubsystemEvent_t	subsys = m_eventMap[subsystem & RPC_SUBSYSTEM_MASK];
	if (subsys == NULL)
	{
		subsys = boost::shared_ptr<boost::unordered_map<uint32_t, ZNPEvent_t> >(new boost::unordered_map<uint32_t, ZNPEvent_t>());
		m_eventMap[subsystem & RPC_SUBSYSTEM_MASK] = subsys;
	}

	ZNPEvent_t cmdEv = (*subsys)[cmd];
	if (cmdEv == NULL)
	{
		cmdEv = boost::shared_ptr<ZNPEvent>(new ZNPEvent());
		(*subsys)[cmd] = cmdEv;
	}

	//_log.Log(_eLogLevel::LOG_STATUS, "%s: adding cb for subsys 0x%02x cmd 0x%02x", __FUNCTION__, subsystem, cmd);
	(*cmdEv) += cb;
}

void ZNPBase::delEventCallback(uint8_t subsystem, uint8_t cmd, boost::shared_ptr<ZNPPacketCb_t> cb)
{
	ZNPSubsystemEvent_t	subsys = m_eventMap[subsystem & RPC_SUBSYSTEM_MASK];
	if (subsys == NULL)
	{
		return;
	}

	ZNPEvent_t cmdEv = (*subsys)[cmd];
	if (cmdEv == NULL)
	{
		return;
	}

	//_log.Log(_eLogLevel::LOG_STATUS, "%s: removing cb for subsys 0x%02x cmd 0x%02x", __FUNCTION__, subsystem, cmd);
	(*cmdEv) -= cb;
}

const char *znpDevState2Str(devStates_t state)
{
	switch (state)
	{
	case DEV_HOLD:
		return "Initialized - not started automatically";
	case DEV_INIT:
		return "Initialized - not connected to anything";
	case DEV_NWK_DISC:
		return "Discovering PAN's to join";
	case DEV_NWK_JOINING:
		return "Joining a PAN";
	case DEV_NWK_SEC_REJOIN_CURR_CHANNEL:
		return "ReJoining a PAN in secure mode scanning in current channel";
	case DEV_END_DEVICE_UNAUTH:
		return "Joined but not yet authenticated by trust center";
	case DEV_END_DEVICE:
		return "Started as device after authentication";
	case DEV_ROUTER:
		return "Device joined, authenticated and is a router";
	case DEV_COORD_STARTING:
		return "Started as Zigbee Coordinator";
	case DEV_ZB_COORD:
		return "Started as Zigbee Coordinator";
	case DEV_NWK_ORPHAN:
		return "Device has lost information about its parent";
	case DEV_NWK_KA:
		return "Device is sending KeepAlive message to its parent";
	case DEV_NWK_BACKOFF:
		return "Device is waiting before trying to rejoin";
	case DEV_NWK_SEC_REJOIN_ALL_CHANNEL:
		return "ReJoining a PAN in secure mode scanning in all channels";
	case DEV_NWK_TC_REJOIN_CURR_CHANNEL:
		return "ReJoining a PAN in Trust center mode scanning in current channel";
	case DEV_NWK_TC_REJOIN_ALL_CHANNEL:
		return "ReJoining a PAN in Trust center mode scanning in all channels";
	default:
		return "undefined state!";
	}
}

//Webserver handlers
namespace http
{
	namespace server
	{
		void CWebServer::RType_GetZStackNodes(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["title"] = "GetZStackNodes";

			CDomoticzHardwareBase *pHardware = m_mainworker.GetHardwareByType(HTYPE_ZNP);
			if (pHardware == NULL)
			{
				root["status"] = "Error";
				return;
			}

			root["status"] = "OK";
			ZNPBase *znp = dynamic_cast<ZNPBase *>(pHardware);
		}

		void CWebServer::RType_GetZStackNwkInfo(WebEmSession & session, const request& req, Json::Value &root)
		{
			root["title"] = "GetZStackNwkInfo";

			CDomoticzHardwareBase *pHardware = m_mainworker.GetHardwareByType(HTYPE_ZNP);
			if (pHardware == NULL)
			{
				root["status"] = "Error";
				return;
			}

			root["status"] = "OK";
			ZNPBase *znp = dynamic_cast<ZNPBase *>(pHardware);
	
			root["channel"] = znp->m_nwkInfo.channel;
			root["panid"] = str(boost::format("0x%04x") % znp->m_nwkInfo.panID);
			root["parentaddress"] = str(boost::format("0x%04x") % znp->m_nwkInfo.parentAddr);
			root["shortaddress"] = str(boost::format("0x%04x") % znp->m_nwkInfo.shortAddr);
			root["extpanid"] = str(boost::format("%02x%02x%02x%02x%02x%02x%02x%02x") 
								% znp->m_nwkInfo.extPanId[7] 
								% znp->m_nwkInfo.extPanId[6]
								% znp->m_nwkInfo.extPanId[5]
								% znp->m_nwkInfo.extPanId[4]
								% znp->m_nwkInfo.extPanId[3]
								% znp->m_nwkInfo.extPanId[2]
								% znp->m_nwkInfo.extPanId[1]
								% znp->m_nwkInfo.extPanId[0]);
			root["state"] = znp->m_nwkInfo.state;
			root["statestr"] = znpDevState2Str(znp->m_nwkInfo.state);
		}
	}
}
