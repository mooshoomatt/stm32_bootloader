
#include "main.h"
#include "uart_hex_loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 1

UART_HEX_OTA_STATUS uart_hex_ota_download(void)
{
	// Initialize ota struct
	UART_HEX_OTA_HANDLE ota;
	memset(ota.rxBuf, '\0', UART_OTA_RX_BUF_LEN);
	ota.base = 0;
	ota.offset = 0;
	ota.eof = 0;

	while(ota.eof == 0)
	{
		receive_hex_line(&ota);
	}

	return UART_HEX_OTA_STATUS_OK;
}

UART_HEX_OTA_STATUS receive_hex_line(UART_HEX_OTA_HANDLE *ota)
{
	uint8_t  nBytes;
	uint32_t base_addr;
	uint32_t offset_addr;
	uint16_t crc;
	uint8_t *rxPtr = ota->rxBuf;
	HEX_FRAME_TYPE frame_type;

	// Wait for start of frame
	uint8_t tmp = 0;
	while (tmp != ':')
	{
		HAL_UART_Receive(&huart3, &tmp, 1, HAL_MAX_DELAY);
	}
	if (DEBUG) { printf("SOF Detected\n"); }

	// Read number of data bytes
	uint8_t nBytes_str[3] = {0};
	HAL_UART_Receive(&huart3, nBytes_str, 2, HAL_MAX_DELAY);
	nBytes = (uint8_t)strtol((char*)nBytes_str, NULL, 16);
	if (DEBUG) { printf("nBytes: %d\n", nBytes); }

	// Read offset
	uint8_t offset_str[5] = {0};
	HAL_UART_Receive(&huart3, offset_str, 4, HAL_MAX_DELAY);
	offset_addr = (uint32_t)strtol((char*)offset_str, NULL, 16);
	if (DEBUG) { printf("Offset: %lu\n", offset_addr); }

	// Read frame type
	uint8_t frame_type_str[3] = {0};
	HAL_UART_Receive(&huart3, frame_type_str, 2, HAL_MAX_DELAY);
	if (!strcmp((char*)frame_type_str, "00")) { frame_type = DAT; }
	if (!strcmp((char*)frame_type_str, "01")) { frame_type = END; ota->eof = 1; }
	if (!strcmp((char*)frame_type_str, "02")) { frame_type = ESA; }
	if (!strcmp((char*)frame_type_str, "03")) { frame_type = SSA; }
	if (!strcmp((char*)frame_type_str, "04")) { frame_type = ELA; }
	if (!strcmp((char*)frame_type_str, "05")) { frame_type = SLA; }
	if (DEBUG) { printf("Frame type: %d\n", frame_type); }

	// Read data
	while (rxPtr < ota->rxBuf + 2*nBytes)
	{
		HAL_UART_Receive(&huart3, rxPtr++, 1, HAL_MAX_DELAY);
	}
	if (DEBUG) { printf("DATA: %s\n", ota->rxBuf); }


	// Read and check CRC
	uint8_t crc_str[3] = {0};
	HAL_UART_Receive(&huart3, crc_str, 2, HAL_MAX_DELAY);
	crc = (uint16_t)strtol((char*)crc_str, NULL, 16);
	// TODO: CHECK CRC
	if (DEBUG) { printf("CRC: %d\n", crc); }

	return UART_HEX_OTA_STATUS_OK;
}
