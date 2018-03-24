#include "stdafx.h"
#include "ZNP_ZCLNode.h"
#include "ZNP_ZCL.h"
#include "ZNPBase.h"
#include "../main/Logger.h"

ZNP_ZCLNode::ZNP_ZCLNode(ZNP_ZCL *owner, uint16_t address) :
	m_owner(owner), m_nodeAddress(address)
{
}

uint8_t ZNP_ZCLNode::getAttributeType(const uint8_t* zclAttribute)
{
	switch (zclAttribute[2])
	{
	case ZCL_DATATYPE_BOOLEAN:
	case ZCL_DATATYPE_UINT8:
	case ZCL_DATATYPE_INT8:
	case ZCL_DATATYPE_BITMAP8:
	case ZCL_DATATYPE_DATA8:
	case ZCL_DATATYPE_ENUM8:
		return 4;
	case ZCL_DATATYPE_UINT16:
	case ZCL_DATATYPE_INT16:
	case ZCL_DATATYPE_DATA16:
	case ZCL_DATATYPE_BITMAP16:
	case ZCL_DATATYPE_ENUM16:
	case ZCL_DATATYPE_SEMI_PREC:
	case ZCL_DATATYPE_CLUSTER_ID:
	case ZCL_DATATYPE_ATTR_ID:
		return 5;
	case ZCL_DATATYPE_UINT32:
	case ZCL_DATATYPE_INT32:
	case ZCL_DATATYPE_DATA32:
	case ZCL_DATATYPE_BITMAP32:
	case ZCL_DATATYPE_SINGLE_PREC:
	case ZCL_DATATYPE_TOD:
	case ZCL_DATATYPE_DATE:
	case ZCL_DATATYPE_UTC:
	case ZCL_DATATYPE_BAC_OID:
		return 7;
	case ZCL_DATATYPE_NO_DATA:
		return 3;
	case ZCL_DATATYPE_DATA64:
	case ZCL_DATATYPE_BITMAP64:
	case ZCL_DATATYPE_UINT64:
	case ZCL_DATATYPE_INT64:
	case ZCL_DATATYPE_DOUBLE_PREC:
	case ZCL_DATATYPE_IEEE_ADDR:
		return 8 + 3;
	case ZCL_DATATYPE_DATA24:
	case ZCL_DATATYPE_BITMAP24:
	case ZCL_DATATYPE_UINT24:
	case ZCL_DATATYPE_INT24:
		return 3 + 3;
	case ZCL_DATATYPE_DATA40:
	case ZCL_DATATYPE_BITMAP40:
	case ZCL_DATATYPE_UINT40:
	case ZCL_DATATYPE_INT40:
		return 5 + 3;
	case ZCL_DATATYPE_DATA48:
	case ZCL_DATATYPE_BITMAP48:
	case ZCL_DATATYPE_UINT48:
	case ZCL_DATATYPE_INT48:
		return 6 + 3;
	case ZCL_DATATYPE_DATA56:
	case ZCL_DATATYPE_BITMAP56:
	case ZCL_DATATYPE_UINT56:
	case ZCL_DATATYPE_INT56:
		return 7 + 3;
	case ZCL_DATATYPE_128_BIT_SEC_KEY:
		return 16 + 3;
	case ZCL_DATATYPE_CHAR_STR:
		return zclAttribute[3] + 3 + 1;
	default:
		_log.Log(_eLogLevel::LOG_STATUS, "unknown data type attr 0x%04x, type: 0x%02x", MAKE_UINT16(zclAttribute), zclAttribute[2]);
		return 0;
	}
}

bool ZNP_ZCLNode::getAttributeValueNum(const uint8_t* zclAttribute, int32_t& value)
{
	switch (zclAttribute[2])
	{
	case ZCL_DATATYPE_BOOLEAN:
	case ZCL_DATATYPE_UINT8:
	case ZCL_DATATYPE_DATA8:
	case ZCL_DATATYPE_ENUM8:
	case ZCL_DATATYPE_BITMAP8:
		value = (int32_t)zclAttribute[3];
		break;
	case ZCL_DATATYPE_INT8:
		value = (int8_t)zclAttribute[3];
		break;
	case ZCL_DATATYPE_UINT16:
	case ZCL_DATATYPE_DATA16:
	case ZCL_DATATYPE_ENUM16:
	case ZCL_DATATYPE_CLUSTER_ID:
	case ZCL_DATATYPE_ATTR_ID:
	case ZCL_DATATYPE_BITMAP16:
		value = MAKE_UINT16(&zclAttribute[3]);
		break;
	case ZCL_DATATYPE_INT16:
		value = (int16_t)MAKE_UINT16(&zclAttribute[3]);
		break;
	case ZCL_DATATYPE_UINT32:
	case ZCL_DATATYPE_DATA32:
	case ZCL_DATATYPE_BITMAP32:
		value = MAKE_UINT32(&zclAttribute[3]);
		break;
	case ZCL_DATATYPE_INT32:
		value = (int32_t)MAKE_UINT32(&zclAttribute[3]);
		break;
	case ZCL_DATATYPE_DATA24:
	case ZCL_DATATYPE_UINT24:
	case ZCL_DATATYPE_BITMAP24:
		value = MAKE_UINT24(&zclAttribute[3]);
		break;
	case ZCL_DATATYPE_INT24:
		value = ((int32_t)(MAKE_UINT24(&zclAttribute[3]) << 8)) / 256;
		break;
	default:
		return false;
	}

	return true;
}

bool ZNP_ZCLNode::getAttributeValueStr(const uint8_t* zclAttribute, std::string& value)
{
	switch (zclAttribute[2])
	{
	case ZCL_DATATYPE_CHAR_STR:
		value = std::string((const char *)&zclAttribute[4], (size_t)zclAttribute[3]);
		break;
	default:
		return false;
	}

	return true;
}

uint8_t ZNP_ZCLNode::decodeAttribute(uint16_t clusterId, const uint8_t *zclAttribute)
{
	uint16_t attrType = MAKE_UINT16(zclAttribute);

	switch (clusterId)
	{
	case ZCL_CLUSTER_ID_GEN_BASIC:
		_log.Log(_eLogLevel::LOG_STATUS, "basic cluster attr 0x%04x", attrType);
		if (attrType == ATTR_TYPE_MANUFACTURER_NAME)
		{
			getAttributeValueStr(zclAttribute, m_manufacturerName);
		}
		else if (attrType == ATTR_TYPE_DEVICE_IDENTIFIER)
		{
			getAttributeValueStr(zclAttribute, m_deviceName);
		}
		break;
	case ZCL_CLUSTER_ID_GEN_ON_OFF:
		_log.Log(_eLogLevel::LOG_STATUS, "ON/OFF cluster attr 0x%04x", attrType);
		if (attrType == ATTR_TYPE_MEAS_VALUE)
		{
			int32_t value;
			if (getAttributeValueNum(zclAttribute, value) == true)
			{
				m_owner->m_znp->SendSwitch(m_nodeAddress, 0, 0xFF, (value != 0), 0, m_manufacturerName + m_deviceName, ZNPBase::LQI2RSSI(m_lqi));
			}
		}
		break;
	case ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT:
		_log.Log(_eLogLevel::LOG_STATUS, "temperature cluster attr 0x%04x", attrType);
		if (attrType == ATTR_TYPE_MEAS_VALUE)
		{
			int32_t value;
			if (getAttributeValueNum(zclAttribute, value) == true)
			{
				m_owner->m_znp->SendTempSensor(m_nodeAddress, 0xFF, value / 100.0f, m_manufacturerName + m_deviceName, ZNPBase::LQI2RSSI(m_lqi));
			}
		}
		break;
	case ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY:
		_log.Log(_eLogLevel::LOG_STATUS, "humidity cluster attr 0x%04x", attrType);
		if (attrType == ATTR_TYPE_MEAS_VALUE)
		{
			int32_t value;
			if (getAttributeValueNum(zclAttribute, value) == true)
			{
				m_owner->m_znp->SendHumiditySensor(m_nodeAddress, 0xFF, value / 100, m_manufacturerName + m_deviceName, ZNPBase::LQI2RSSI(m_lqi));
			}
		}
		break;
	case ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING:
		_log.Log(_eLogLevel::LOG_STATUS, "OCCUPANCYSENSING cluster attr 0x%04x", attrType);
		if (attrType == ATTR_TYPE_MEAS_VALUE)
		{
			int32_t value;
			if (getAttributeValueNum(zclAttribute, value) == true)
			{
				m_owner->m_znp->SendSwitch(m_nodeAddress, 0, 0xFF, (value != 0), 0, m_manufacturerName + m_deviceName, ZNPBase::LQI2RSSI(m_lqi));
			}
		}
		break;
	default:
		_log.Log(_eLogLevel::LOG_STATUS, "unknown cluster id 0x%04x, attr 0x%04x", clusterId, attrType);
		break;
	}

	return getAttributeType(zclAttribute);
}

void ZNP_ZCLNode::readAttributes(uint16_t clusterId, const uint8_t *zclFramePayload, uint8_t length)
{
	int32_t len = length;
	int32_t attrSize;
	std::string	str;

	while (len > 0)
	{
		attrSize = decodeAttribute(clusterId, zclFramePayload);
		if (attrSize == 0)
		{
			break;
		}
		len -= attrSize;
		zclFramePayload += attrSize;
	}
}

void ZNP_ZCLNode::OnNewZCLFrame(uint16_t clusterId, const uint8_t *zclFrame, uint8_t length)
{
	uint8_t dataLen;

	if (zclFrame[0] & ZCL_FRAME_CONTROL_MANU_SPECIFIC)
	{
		zclFrame += 3;
		dataLen = length - 5;
	}
	else
	{
		dataLen = length - 3;
		zclFrame++;
	}

	if ((m_frameSeq + 1) != *zclFrame)
	{
		_log.Log(_eLogLevel::LOG_STATUS, "ZCL frame seq: 0x%02x != 0x%02x mismath!", m_frameSeq, *zclFrame);
	}
	m_frameSeq = *zclFrame++;

	_log.Log(_eLogLevel::LOG_STATUS, "ZCL cmdID: 0x%02x", *zclFrame);

	if (*zclFrame == ZCL_CMD_REPORT)
	{
		readAttributes(clusterId, ++zclFrame, dataLen);
	}
	else
	{
		_log.Log(_eLogLevel::LOG_STATUS, "ZCL cmdID: 0x%02x not implemented!", *zclFrame);
	}
}