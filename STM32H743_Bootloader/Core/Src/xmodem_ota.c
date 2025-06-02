#include "xmodem_ota.h"

static uint32_t xm_current_address		 = APP_START_ADDRESS;
static uint8_t  xm_packet_number 		 = 0x1;
static uint8_t  xm_first_header_received = 0x0; // Flag indicating transfer has started
static uint8_t  xm_flash_erased = 0x0; 			// Flag indicating we have erased flash

static uint8_t  packet_num_buf[X_PACKET_NUM_SIZE]   = {0};
static uint8_t  data_buf[X_DATA_SIZE]	 			= {0};
static uint8_t  checksum_buf[X_CHECKSUM_SIZE]       = {0};

void xmodem_receive_ota();
static XMODEM_Status xmodem_handle_packet();
static XMODEM_Status xmodem_handle_error(uint8_t *error_count);
static uint8_t xmodem_calc_checksum(uint8_t *pData, uint16_t length);

void xmodem_receive_ota(void)
{
	volatile XMODEM_Status status = X_OK;
	uint8_t error_count = 0;

	while (status == X_OK)
	{
		uint8_t header = 0;
		UART_Status comm_status = UART_OK;

		// Receive header
		if (xm_first_header_received == 1)
		{
			// Normal mid-transfer header receive
			comm_status = uart_receive(&header, 1);
		}
		else
		{
			// Spam host and wait for first header
			while (uart_receive(&header, 1) != UART_OK)
			{
				// Send NAK for start of transfer
				uart_transmit_ch(X_NAK);
			}
			xm_first_header_received = 1;
		}

		// Check for UART timeout or other errors
		if (comm_status != UART_OK)
		{
			status = xmodem_handle_error(&error_count);
		}

		// Switch based on header content
		switch (header)
		{
			case X_SOH:
				// Start of header
				XMODEM_Status packet_status = xmodem_handle_packet();
				// If success, send ACK
				if (packet_status == X_OK)
				{
					uart_transmit_ch(X_ACK);
				}
				// If flash error, then abort immediately
				else if (packet_status == X_ERROR_FLASH)
				{
					error_count = X_MAX_ERRORS;
					status = xmodem_handle_error(&error_count);
				}
				// Otherwise, send NAK / abort if max error count reached
				else
				{
					status = xmodem_handle_error(&error_count);
				}
				break;
			case X_EOT:
				// End of transfer
				uart_transmit_ch(X_ACK);
				printf("%d Packets Processed\n", xm_packet_number-1);
				printf("Firmware Update Complete!\n");
				return;
			case X_CAN:
				// Cancel transfer
				status = X_ERROR;
				break;
			default:
				// Wrong header
				if (comm_status == UART_OK)
				{
					status = xmodem_handle_error(&error_count);
				}
				break;
		}
	}
}

static XMODEM_Status xmodem_handle_packet()
{
	XMODEM_Status status = X_OK;
	UART_Status comm_status = UART_OK;
	Flash_Status flash_status = FLASH_OK;

	// Read packet contents into buffers
	comm_status |= uart_receive(packet_num_buf, X_PACKET_NUM_SIZE);
	comm_status |= uart_receive(data_buf, X_DATA_SIZE);
	comm_status |= uart_receive(checksum_buf, X_CHECKSUM_SIZE);

	// Check for comm error
	if (comm_status != UART_OK)
	{
		status |= X_ERROR_UART;
	}

	// If we haven't yet, erase the flash
	if ((status == X_OK) && (xm_flash_erased == 0))
	{
		flash_status = flash_erase(APP_START_SECTOR, APP_END_SECTOR);
		if (flash_status == FLASH_OK)
		{
			xm_flash_erased = 1;
		}
		else
		{
			status |= X_ERROR_FLASH;
		}
	}

	// Check packet number
	uint8_t received_packet_num		 = packet_num_buf[0];
	uint8_t received_packet_num_comp = packet_num_buf[1];
	if (received_packet_num != xm_packet_number)
	{
		status |= X_ERROR_NUMBER;
	}
	if ((received_packet_num + received_packet_num_comp) != 0xFF)
	{
		status |= X_ERROR_NUMBER;
	}

	// Check checksum
	uint8_t checksum_calc = xmodem_calc_checksum(data_buf, X_DATA_SIZE);
	if (checksum_calc != *checksum_buf)
	{
		status |= X_ERROR_CRC;
	}

	// If things look good, proceed to flash
	if (status == X_OK)
	{
		// 128 bytes = 1024 bits = 4 flash words
		flash_status = flash_write(xm_current_address, (uint32_t*)data_buf, 4);

		if (flash_status == FLASH_OK)
		{
			xm_packet_number++;
			xm_current_address += X_DATA_SIZE;
		}
		else
		{
			status |= X_ERROR_FLASH;
		}
	}

	return status;
}

static XMODEM_Status xmodem_handle_error(uint8_t *error_count)
{
	XMODEM_Status status = X_OK;
	// Increment error counter
	(*error_count)++;
	// Check if we have hit the max error count
	if (*error_count >= X_MAX_ERRORS)
	{
		// Abort transfer
		uart_transmit_ch(X_CAN);
		HAL_Delay(10); // Is this delay needed?
		uart_transmit_ch(X_CAN);
		status = X_ERROR;
	}
	// Otherwise send a NAK for repeat
	else
	{
		uart_transmit_ch(X_NAK);
	}
	return status;
}

static uint8_t xmodem_calc_checksum(uint8_t *pData, uint16_t length)
{
	uint8_t checksum = 0;
	for (uint16_t i = 0; i < X_DATA_SIZE; i++)
	{
		checksum += pData[i];
	}
	return checksum;
}
