#include "xmodem_ota.h"

static uint32_t xm_address				 = APP_START_ADDRESS;
static uint8_t  xm_packet_number 		 = 0x1;
static uint8_t  xm_first_packet_received = 0x0;
uint8_t rxBuf[X_PACKET_SIZE];

//static uint16_t xmodem_calc_crc(uint8_t *pData, uint16_t length);
//static XMODEM_Status xmodem_handle_packet(uint8_t size);
//static XMODEM_Status xmodem_error_handler(uint8_t *error_number, uint8_t max_error_number);

void xmodem_receive_ota(void)
{
	volatile XMODEM_Status status = X_OK;
	uint8_t error_number = 0;

	while (status == X_OK)
	{

		// If we haven't started the transfer yet
		if (xm_first_packet_received == 0)
		{
			uart_transmit_ch(X_NAK);
		}

		// Receive packet
		if (uart_receive(rxBuf, X_PACKET_SIZE) == UART_OK)
		{
			xm_first_packet_received = 1;
			while(1);
		}
	}

}
