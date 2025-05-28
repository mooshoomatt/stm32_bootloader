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
3. Read application vector table
4. Jump to application 

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