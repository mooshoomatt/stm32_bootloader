#include "uart.h"

UART_Status uart_receive(uint8_t *pData, uint16_t length)
{
	UART_Status status = UART_ERROR;
	if (HAL_UART_Receive(&huart3, pData, length, UART_TIMEOUT) == HAL_OK)
	{
		status = UART_OK;
	}
	return status;
}

UART_Status uart_transmit(uint8_t *pData, uint16_t length)
{
	UART_Status status = UART_ERROR;
	if (HAL_UART_Transmit(&huart3, pData, length, UART_TIMEOUT) == HAL_OK)
	{
		status = UART_OK;
	}
	return status;
}

UART_Status uart_transmit_str(uint8_t *pData)
{
	UART_Status status = UART_ERROR;
	uint16_t length = 0;
	while (pData[length] != '\0')
	{
		length++;
	}
	if (HAL_UART_Transmit(&huart3, pData, length, UART_TIMEOUT) == HAL_OK)
	{
		status = UART_OK;
	}
	return status;
}

UART_Status uart_transmit_ch(uint8_t data)
{
	UART_Status status = UART_ERROR;
	if (HAL_UART_Transmit(&huart3, &data, 1, UART_TIMEOUT) == HAL_OK)
	{
		status = UART_OK;
	}
	return status;
}
