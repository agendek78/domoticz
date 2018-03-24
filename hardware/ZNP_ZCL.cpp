#include "stdafx.h"
#include "ZNP_ZCL.h"
#include "../main/Logger.h"
#include "ZNPBase.h"

#include <string>



void ZNP_ZCL::dumpBuffer(const uint8_t *data, uint8_t len)
{
	const uint8_t *ptr = data;
	
	while (len > 0)
	{
		if (len == 1)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x", ptr[0]);
			break;
		}
		else if (len == 2)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x", ptr[0], ptr[1]);
			break;
		}
		else if (len == 3)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x", ptr[0], ptr[1], ptr[2]);
			break;
		}
		else if (len == 4)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3]);
			break;
		}
		else if (len == 5)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4]);
			break;
		}
		else if (len == 6)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
			break;
		}
		else if (len == 7)
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x %02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6]);
			break;
		}
		else
		{
			_log.Log(_eLogLevel::LOG_STATUS, "%02x %02x %02x %02x %02x %02x %02x %02x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);
			len -= 8;
			ptr = &ptr[8];
		}
	}
}

ZNP_ZCL::ZNP_ZCL(ZNPBase *znp) :
	m_znp(znp)
{

}

void ZNP_ZCL::OnNewPacket(const ZNPPacket_t *packet)
{
	uint16_t clusterId = MAKE_UINT16(&packet->data[2]);
	uint16_t nodeAddr = MAKE_UINT16(&packet->data[4]);

	boost::shared_ptr<ZNP_ZCLNode>	node = m_nodes[nodeAddr];

	if (node == NULL)
	{
		node = boost::shared_ptr<ZNP_ZCLNode>(new ZNP_ZCLNode(this, nodeAddr));
		m_nodes[nodeAddr] = node;
	}

	node->m_lqi = packet->data[9];

	_log.Log(_eLogLevel::LOG_STATUS, "New ZCL data packet for node 0x%04x, LQI 0x%02x", nodeAddr, packet->data[9]);

	dumpBuffer(packet->data, packet->length);

#if 0
	_log.Log(_eLogLevel::LOG_STATUS, "groupID: 0x%04x", MAKE_UINT16(packet->data));
	_log.Log(_eLogLevel::LOG_STATUS, "clusterID: 0x%04x", m_clusterID);
	_log.Log(_eLogLevel::LOG_STATUS, "srcAddr: 0x%04x", m_srcAddr);
	_log.Log(_eLogLevel::LOG_STATUS, "srcEP: 0x%02x", packet->data[6]);
	_log.Log(_eLogLevel::LOG_STATUS, "dstEP: 0x%02x", packet->data[7]);
	_log.Log(_eLogLevel::LOG_STATUS, "isBcst: 0x%02x", packet->data[8]);
#endif



//	_log.Log(_eLogLevel::LOG_STATUS, "LQI: 0x%02x", m_lqi);
//	_log.Log(_eLogLevel::LOG_STATUS, "seq: 0x%02x", packet->data[15]);

	uint8_t len = packet->data[16];
//	_log.Log(_eLogLevel::LOG_STATUS, "ZCL len: %d", len);

	if (len > 0)
	{
		node->OnNewZCLFrame(clusterId, &packet->data[17], len);
	}
	else
	{
		_log.Log(_eLogLevel::LOG_STATUS, "ZCL frame length is 0!");
	}
}
