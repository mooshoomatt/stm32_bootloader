#include "flash.h"

/**
 * Function: flash_erase
 * ---------------------
 * Erases sector(s) of flash
 *
 * start_sector: 	first sector to erase
 * end_sector: 		last sector to erase
 *
 * returns: 	flash status enum
 *
 */
Flash_Status flash_erase(uint32_t start_sector, uint32_t end_sector)
{
	Flash_Status status = FLASH_OK;

	if (HAL_FLASH_Unlock() != HAL_OK)
	{
		status = FLASH_ERROR;
	}

	FLASH_EraseInitTypeDef erase_init;
	uint32_t sectorError = 0;

	erase_init.TypeErase 	= FLASH_TYPEERASE_SECTORS;
	erase_init.Banks 	 	= FLASH_BANK_1;
	erase_init.Sector 	 	= start_sector;
	erase_init.NbSectors 	= (end_sector - start_sector + 0x1);
	erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	if (HAL_FLASHEx_Erase(&erase_init, &sectorError) != HAL_OK)
	{
		status = FLASH_ERROR;
	}

	if (HAL_FLASH_Lock() != HAL_OK)
	{
		status = FLASH_ERROR;
	}

	return status;
}

/**
 * Function: flash_write
 * ---------------------
 * Writes data to flash
 *
 * address: 	flash address to program to
 * pData: 		pointer to source data
 * length: 		number of 256 bit words to write
 *
 * returns: 	flash status enum
 *
 */
Flash_Status flash_write(uint32_t address, uint32_t *pData, uint32_t length)
{
	Flash_Status status = FLASH_OK;

	// Check data alignment - must be aligned to 32 byte words (256 bits)
	if ((address % 0x20) != 0) { status |= FLASH_ERROR_ALIGN; }

	// Check address
	if (address > APP_END_ADDRESS) { status |= FLASH_ERROR_OVER; }

	// Unlock flash
	if (HAL_FLASH_Unlock() != HAL_OK) { status |= FLASH_ERROR_LOCK; }

	// Write to flash
	for (uint32_t i = 0; (i < length) && (status == FLASH_OK); i++)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address + i*32, (uint32_t)(pData + i*8)) != HAL_OK)
		{
			status |= FLASH_ERROR_WRITE;
		}
	}

	// Verify write
	for (uint32_t i = 0; i < length*8; i++)
	{
		if (*(uint32_t*)(address + i*4) != pData[i])
		{
			status |= FLASH_ERROR_CHECK;
		}
	}

	if (HAL_FLASH_Lock() != HAL_OK)
	{
		status |= FLASH_ERROR_LOCK;
	}

	return status;
}
