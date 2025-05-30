#include "flash.h"

Flash_Status flash_erase()
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
	erase_init.Sector 	 	= APP_START_SECTOR;
	erase_init.NbSectors 	= (APP_END_SECTOR - APP_START_SECTOR + 0x1);
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


Flash_Status flash_write(uint32_t address, uint32_t *pData, uint32_t length)
{
	Flash_Status status = FLASH_OK;

	if (HAL_FLASH_Unlock() != HAL_OK)
	{
		status = FLASH_ERROR;
	}

	for (uint32_t i = 0; (i < length) && (status == FLASH_OK); i++)
	{
		if (address > APP_END_ADDRESS)
		{
			status |= FLASH_ERROR_OVER;
		}
		else
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address, pData[i]) != HAL_OK)
			{
				status |= FLASH_ERROR_WRITE;
			}
			if (*(volatile uint32_t*)address != pData[i])
			{
				status |= FLASH_ERROR_CHECK;
			}
			address += 4;
		}
	}

	if (HAL_FLASH_Lock() != HAL_OK)
	{
		status = FLASH_ERROR;
	}

	return status;
}
