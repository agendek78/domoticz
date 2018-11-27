#pragma once
#include <stdint.h>
#include <boost/function.hpp>

#define MAX_ZNP_PACKET_LEN	(255)
#define MAX_ZNP_DATA_LEN	(250)

#define GFF_HEADER_LEN			(3u)
#define UART_FRAME_HEADER_LEN	(1u)
#define UART_FRAME_FCS_LEN		(1u)
#define ZNP_FRAME_HEAD_FT_LEN	(GFF_HEADER_LEN + UART_FRAME_HEADER_LEN + UART_FRAME_FCS_LEN)
#define ZNP_FRAME_LEN_POS		(1u)

// Start of frame character value
#define RPC_UART_SOF        0xFE

typedef struct ZNPPacket_s
{
	uint8_t			length;
	uint8_t			cmdSubSystem;
	uint8_t			cmdID;
	const uint8_t	*data;

	ZNPPacket_s() {};
	ZNPPacket_s(const uint8_t *buff) { data = buff; };
} ZNPPacket_t;

typedef boost::function<void(const ZNPPacket_t *packet)> ZNPPacketCb_t;

class ZNPParser
{
	uint8_t			m_packetBuffer[MAX_ZNP_PACKET_LEN];
	uint32_t		m_dataLen;
	ZNPPacket_t		m_currPacket;

	ZNPPacketCb_t	onNewZNPPacket;

	int32_t preparePacket(const uint8_t *data, uint32_t len);
	int32_t scanBuffer(const uint8_t *data, uint32_t len);

public:
	ZNPParser(ZNPPacketCb_t cb);
	~ZNPParser();

	void ZNPDataPush(const uint8_t *data, uint32_t len);

	static uint8_t calcFCS(const uint8_t *pMsg, uint32_t len);
};
