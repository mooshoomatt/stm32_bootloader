#include "main.h"

#define UART_OTA_RX_BUF_LEN 256

typedef enum
{
	DAT = 0,
	END = 1,
	ESA = 2,
	SSA = 3,
	ELA = 4,
	SLA = 5,
} HEX_FRAME_TYPE;

typedef enum
{
	UART_HEX_OTA_STATUS_OK = 0,
	UART_HEX_OTA_STATUS_ERR = 1,
} UART_HEX_OTA_STATUS;

typedef struct
{
	uint8_t rxBuf[UART_OTA_RX_BUF_LEN];
	uint32_t base;
	uint32_t offset;
	uint32_t eof;
} UART_HEX_OTA_HANDLE;

UART_HEX_OTA_STATUS uart_hex_ota_download(void);
UART_HEX_OTA_STATUS receive_hex_line(UART_HEX_OTA_HANDLE *ota);
