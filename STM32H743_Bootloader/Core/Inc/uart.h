#ifndef UART_H_
#define UART_H_

#include "stm32h7xx_hal.h"

extern UART_HandleTypeDef huart3;

#define UART_TIMEOUT ((uint16_t)1000u)

typedef enum
{
	UART_OK 	= 0x0,
	UART_ERROR  = 0x1,
} UART_Status ;

UART_Status uart_receive(uint8_t *pData, uint16_t length);
UART_Status uart_transmit(uint8_t *pData, uint16_t length);
UART_Status uart_transmit_str(uint8_t *pData);
UART_Status uart_transmit_ch(uint8_t data);

#endif /* UART_H_ */
