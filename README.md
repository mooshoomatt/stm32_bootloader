# STM32H743 Bootloader
Following along with YouTube tutorials by EmbeTronicX\
Part 1: https://www.youtube.com/watch?v=jzo7z2gNBgg&list=PLArwqFvBIlwHRgPtsQAhgZavlp42qpkiG

# Overview
The general idea is to think of the bootloader and application as two separate projects/binaries, and we simply put them in different sections in flash.\
Looking at the MCU reference manual, we can inspect the flash layout of this particular part number:\
![STM32H743 Flash Layout](/images/stm32h743_flash_layout.JPG)\
The 2MB of flash is divided into two 1MB "banks," each of which is divided into eight 128KB "sectors."\
It seems to me that only entire sectors can be erased/written at a time, so maybe it's good practice to divide everything along these sectors, or multiples of them? It seems like it, at least. 

# Bootloader
## High Level Summary
1. Initialize clocks
2. Initialize peripherals we want in the bootloader 
3. Check for OTA request from user (or later, from the application)
    - If there is an OTA request, receive update, flash, and reset 
4. Read application vector table
5. Jump to application 

## Implementation details
The clock and peripheral initialization is pretty much taken care of by the code generation tools, so not much to do there.\
One small thing that is nice is to override the printf function and send it to our virtual COM port. This little piece of code takes care of that:
```
/*
 * @brief 	Print the characters to UART (printf).
 * 			With GCC, small printf (options LD Linker->Libraries->Small printf
 * 			set to 'Yes') calls __io_putchar()
 *
 * @retval int
 */
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *F)
#endif /* __GNUC__ */
{
	/* Place your implementation of __io_putchar/fputc here */
	HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
	return ch;
}
```
To check for an OTA update request, we just read the state of a GPIO pin (in this case, the user button on PC13). If an update is received, we call the OTA update routine.\
EmbeTronicX uses something called ETX OTA protocol for this, which requires using a separate UART for bidirectional communication + handshaking. Personally, I am more used to being able to use the same debug serial interface to send the firmware update (typically transmitting HEX or SREC), so I am going to try and implement that instead. TBD on how that goes.\

We can make the STM32CubeIDE automatically generate a HEX file for our binary by adding the following command to Project > Properties > C/C++ Build > Settings > Build steps > Post-build steps > Command:
```
arm-none-eabi-objcopy -O ihex ${ProjName}.elf ${ProjName}.hex
```

# Application
In this case we don't really care what this is. The app as it currently exists just blinks the yellow LED on my NUCLEO-H743ZI2 at 2Hz.\
The only "weird" part about this project is that we had to modify the linker script (STM32H743ZITX_FLASH.ld) to everything at an offset (in this case, 0x40000 or 512KB) which is pretty trivial.
```
/* Specify the memory areas */
MEMORY
{
  FLASH (rx)     : ORIGIN = 0x08040000, LENGTH = 512K /* was 0x08000000, 2048K */
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
}
```
We also need to relocate the vector table (core/src/system_stm32h7xx.c) with the same offset.
```
#define USER_VECT_TAB_ADDRESS
#define VECT_TAB_OFFSET         0x00040000U       /*!< Vector Table base offset field.
                                                       This value must be a multiple of 0x400. */
```