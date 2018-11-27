#include "stdafx.h"
#include "ZNPParser.h"

#include <stdlib.h>
#include <boost/assert.hpp>

ZNPParser::ZNPParser(ZNPPacketCb_t cb) :
	onNewZNPPacket(cb), m_dataLen(0)
{

}

ZNPParser::~ZNPParser()
{

}

uint8_t ZNPParser::calcFCS(const uint8_t *pMsg, uint32_t len)
{
	uint8_t result = 0;
	while (len--)
	{
		result ^= *pMsg++;
	}
	return result;
}

static uint32_t getSOF(const uint8_t *data, uint32_t len)
{
	uint32_t i = 0;
	while (i < len)
	{
		if (data[i] == RPC_UART_SOF)
		{
			break;
		}
		i++;
	}

	return i; 
}

int32_t ZNPParser::preparePacket(const uint8_t *data, uint32_t len)
{
	if (len > ZNP_FRAME_LEN_POS && (data[ZNP_FRAME_LEN_POS] + ZNP_FRAME_HEAD_FT_LEN) <= len)
	{
		if (calcFCS(&data[ZNP_FRAME_LEN_POS], data[ZNP_FRAME_LEN_POS] + GFF_HEADER_LEN) == data[data[ZNP_FRAME_LEN_POS] + GFF_HEADER_LEN + UART_FRAME_FCS_LEN])
		{
			m_currPacket.length = data[ZNP_FRAME_LEN_POS];
			m_currPacket.cmdSubSystem = data[2] & 0x1Fu;
			m_currPacket.cmdID = data[3];			
			if (m_currPacket.length != 0)
			{
				m_currPacket.data = &data[4];
			}
			else
			{
				m_currPacket.data = NULL;
			}
			return data[1] + ZNP_FRAME_HEAD_FT_LEN;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

int32_t ZNPParser::scanBuffer(const uint8_t *data, uint32_t len)
{
	uint32_t		sof = 0;
	const uint8_t	*frame = data;
	uint32_t		bytesLeft = len;
	int32_t			bytesUsed;

	while (sof < bytesLeft)
	{
		sof = getSOF(frame, bytesLeft);
		frame = &frame[sof];
		bytesLeft -= sof;

		bytesUsed = preparePacket(frame, bytesLeft);
		if (bytesUsed == 0)
		{
			//not enought data to complete packet
			if (bytesLeft > 0)
			{
				memcpy(m_packetBuffer, frame, bytesLeft);
				m_dataLen = (uint32_t)bytesLeft;
			}
			return 1;
		}
		else if (bytesUsed < 0)
		{
			//error verifying packet data
			//move to next byte
			frame++;
			bytesLeft--;
		}
		else
		{
			onNewZNPPacket(&m_currPacket);
			frame = &frame[bytesUsed];
			bytesLeft -= bytesUsed;
		}
	}

	return 0;
}

void ZNPParser::ZNPDataPush(const uint8_t *data, uint32_t len)
{
	if (m_dataLen == 0)
	{
		scanBuffer(data, len);
	}
	else
	{
		uint32_t pktLen;
		const uint8_t *frame = data;
		uint32_t dataLeft = len;

		while (dataLeft > 0)
		{
			BOOST_ASSERT(m_packetBuffer[0] == RPC_UART_SOF);
			BOOST_ASSERT(m_dataLen > 0);

			if (m_dataLen == ZNP_FRAME_LEN_POS)
			{
				pktLen = frame[0];
			}
			else
			{
				pktLen = m_packetBuffer[ZNP_FRAME_LEN_POS];
			}

			if (pktLen <= (m_dataLen - ZNP_FRAME_HEAD_FT_LEN + dataLeft))
			{
				uint32_t dataUsed = pktLen - m_dataLen + ZNP_FRAME_HEAD_FT_LEN - UART_FRAME_HEADER_LEN;

				memcpy(&m_packetBuffer[m_dataLen], frame, dataUsed);
				m_dataLen += dataUsed;

				dataLeft -= dataUsed;
				frame = &frame[dataUsed];

				if (scanBuffer(m_packetBuffer, m_dataLen) == 0)
				{
					m_packetBuffer[0] = 0;
					m_dataLen = 0;
					scanBuffer(frame, dataLeft);
					break;
				}
				else if (dataUsed == 0)
				{
					//move to next byte
					if (scanBuffer(&m_packetBuffer[1], m_dataLen - 1) == 0)
					{
						m_packetBuffer[0] = 0;
						m_dataLen = 0;
						scanBuffer(frame, dataLeft);
						break;
					}
				}
			}
			else
			{
				memcpy(&m_packetBuffer[m_dataLen], frame, dataLeft);
				m_dataLen += dataLeft;
				break;
			}
		}
	}
}
