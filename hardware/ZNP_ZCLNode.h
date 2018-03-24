#pragma once

#include <stdint.h>
#include <string>
#include <Boost/unordered_map.hpp>

#define ATTR_TYPE_MEAS_VALUE	 0x0000
#define ATTR_TYPE_MIN_MEAS_VALUE 0x0001
#define ATTR_TYPE_MAX_MEAS_VALUE 0x0003
#define ATTR_TYPE_MEAS_TOLERANCE 0x0004

#define ATTR_TYPE_MANUFACTURER_NAME	0x0004
#define ATTR_TYPE_DEVICE_IDENTIFIER	0x0005

typedef enum
{
	ZCLNodePowerSource_Unknown,
	ZCLNodePowerSource_Mains,
	ZCLNodePowerSource_Mains3PH,
	ZCLNodePowerSource_Battery,
	ZCLNodePowerSource_DCSource,
	ZCLNodePowerSource_EmergencyMains,
	ZCLNodePowerSource_EmergencyMainsTrSwitch
} ZCLNodePowerSource_t;

class ZNP_ZCL;

class ZNP_ZCLNode
{
	friend class ZNP_ZCL;

	ZNP_ZCL					*m_owner;
	uint16_t				m_nodeAddress;
	std::string				m_manufacturerName;
	std::string				m_deviceName;
	uint8_t					m_lqi;
	uint8_t					m_batteryRemaining;
	ZCLNodePowerSource_t	m_powerSource;
	uint8_t					m_frameSeq;

	bool	getAttributeValueStr(const uint8_t* zclAttribute, std::string& value);
	bool    getAttributeValueNum(const uint8_t* zclAttribute, int32_t& value);
	uint8_t getAttributeType(const uint8_t* zclAttribute);
	uint8_t decodeAttribute(uint16_t clusterId, const uint8_t *zclAttribute);
	void	readAttributes(uint16_t clusterId, const uint8_t *zclFramePayload, uint8_t length);

public:
	ZNP_ZCLNode(ZNP_ZCL *owner, uint16_t address);

	void OnNewZCLFrame(uint16_t clusterId, const uint8_t *zclFrame, uint8_t length);
};