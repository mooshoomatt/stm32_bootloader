/**
 * @file	xmodem_ota.h
 * @author	Matthew Morawiec
 * @date	05 May 2025
 * @brief 	This module implements the Xmodem protocol for OTA programming
 *
 * Used work by Ferenc Nemeth for inspiration: https://github.com/ferenc-nemeth/stm32-bootloader/
 *
 */

#ifndef XMODEM_OTA_H_
#define XMODEM_OTA_H_

#include "uart.h"
#include "flash.h"

/* Xmodem (128 bytes) packet format
 * Byte  0:       Header
 * Byte  1:       Packet number
 * Byte  2:       Packet number complement
 * Bytes 3-130:   Data
 * Bytes 131-132: CRC
 */

#define X_MAX_ERRORS ((uint8_t)10u)

#define X_PACKET_SIZE     ((uint8_t)132u)
#define X_PACKET_NUM_SIZE ((uint8_t)2u)
#define X_PACKET_128_SIZE ((uint16_t)128u)
#define X_PACKET_CRC_SIZE ((uint16_t)2u)

#define X_SOH ((uint8_t)0x01u)
#define X_EOT ((uint8_t)0x04u)
#define X_ACK ((uint8_t)0x06u)
#define X_NAK ((uint8_t)0x15u)
#define X_CAN ((uint8_t)0x18u)

typedef enum
{
	X_OK 			= 0x00,
	X_ERROR_CRC 	= 0x01,
	X_ERROR_NUMBER	= 0x02,
	X_ERROR_UART	= 0x04,
	X_ERROR_FLASH	= 0x08,
	X_ERROR			= 0xFF,
} XMODEM_Status;

void xmodem_receive_ota(void);

#endif /* XMODEM_OTA_H_ */
