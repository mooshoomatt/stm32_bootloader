#ifndef FLASH_H_
#define FLASH_H_

#include "stm32h7xx_hal.h"

#define APP_START_ADDRESS ((uint32_t)0x08040000)  // Start of bank 1 sector 3
#define APP_END_ADDRESS   ((uint32_t)0x080FFFFF)  // End of bank 1 sector 7
#define APP_START_SECTOR  FLASH_SECTOR_2
#define APP_END_SECTOR	  FLASH_SECTOR_7


typedef enum {
	FLASH_OK 			= 0x00,  // Success
	FLASH_ERROR_OVER 	= 0x01,  // Binary too big
	FLASH_ERROR_WRITE	= 0x02,  // Write failed
	FLASH_ERROR_CHECK	= 0x04,  // Readback failed
	FLASH_ERROR			= 0xFF,  // Generic failure
} Flash_Status;

Flash_Status flash_erase();
Flash_Status flash_write(uint32_t address, uint32_t *pData, uint32_t length);

#endif /* FLASH_H_ */
