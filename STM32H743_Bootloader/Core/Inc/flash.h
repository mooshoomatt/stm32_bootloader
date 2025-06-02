#ifndef FLASH_H_
#define FLASH_H_

#include "stm32h7xx_hal.h"

#define APP_START_ADDRESS 	((uint32_t)0x08040000)  // Start of bank 1 sector 3
#define APP_END_ADDRESS   	((uint32_t)0x080FFFFF)  // End of bank 1 sector 7
#define APP_START_SECTOR  	FLASH_SECTOR_2
#define APP_END_SECTOR	  	FLASH_SECTOR_7

typedef enum {
	FLASH_OK 			= 0x00,  // Success
	FLASH_ERROR_OVER 	= 0x01,  // Binary too big
	FLASH_ERROR_WRITE	= 0x02,  // Write failure
	FLASH_ERROR_CHECK	= 0x04,  // Readback failure
	FLASH_ERROR_ALIGN	= 0x08,  // Align failure
	FLASH_ERROR_LOCK	= 0x10,  // Lock failure
	FLASH_ERROR			= 0x80,  // Generic failure
} Flash_Status;

Flash_Status flash_erase(uint32_t start_sector, uint32_t end_sector);
Flash_Status flash_write_256b(uint32_t address, uint32_t *pData);
Flash_Status flash_write_128b(uint32_t address, uint32_t *pData);
Flash_Status flash_write(uint32_t address, uint32_t *pData, uint32_t length);

#endif /* FLASH_H_ */
