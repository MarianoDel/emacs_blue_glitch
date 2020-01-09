#ifndef FLASH_PROGRAM_H_
#define FLASH_PROGRAM_H_

#include "GTK_Estructura.h"
#include "stm32f10x.h"

#define SIZE_SLOT	82
#define SLOT_BASE_ADDR (uint32_t)0x08060000
#define SLOT1_START_ADDR 	(SLOT_BASE_ADDR) //84 bytes.
#define SLOT2_START_ADDR 	(SLOT_BASE_ADDR + 1 * SIZE_SLOT) //84 bytes.
#define SLOT3_START_ADDR 	(SLOT_BASE_ADDR + 2 * SIZE_SLOT) //84 bytes.
#define SLOT4_START_ADDR 	(SLOT_BASE_ADDR + 3 * SIZE_SLOT) //84 bytes.
#define SLOT5_START_ADDR 	(SLOT_BASE_ADDR + 4 * SIZE_SLOT) //84 bytes.
#define SLOT6_START_ADDR 	(SLOT_BASE_ADDR + 5 * SIZE_SLOT) //84 bytes.
#define SLOT7_START_ADDR 	(SLOT_BASE_ADDR + 6 * SIZE_SLOT) //84 bytes.
#define SLOT8_START_ADDR 	(SLOT_BASE_ADDR + 7 * SIZE_SLOT) //84 bytes.
#define SLOT9_START_ADDR 	(SLOT_BASE_ADDR + 8 * SIZE_SLOT) //84 bytes.
#define SLOT10_START_ADDR 	(SLOT_BASE_ADDR + 9 * SIZE_SLOT) //84 bytes.
#define SLOT11_START_ADDR 	(SLOT_BASE_ADDR + 10 * SIZE_SLOT) //84 bytes.
#define SLOT12_START_ADDR 	(SLOT_BASE_ADDR + 11 * SIZE_SLOT) //84 bytes.
#define SLOT13_START_ADDR 	(SLOT_BASE_ADDR + 12 * SIZE_SLOT) //84 bytes.
#define SLOT14_START_ADDR 	(SLOT_BASE_ADDR + 13 * SIZE_SLOT) //84 bytes.
#define SLOT15_START_ADDR 	(SLOT_BASE_ADDR + 14 * SIZE_SLOT) //84 bytes.
#define SLOT16_START_ADDR 	(SLOT_BASE_ADDR + 15 * SIZE_SLOT) //84 bytes.


/*#define SLOT1_START_ADDR 	0x8060000
#define SLOT2_START_ADDR 	0x8060060
#define SLOT3_START_ADDR 	0x80600C0
#define SLOT4_START_ADDR 	0x8060120
#define SLOT5_START_ADDR 	0x8060180
#define SLOT6_START_ADDR 	0x80601E0
#define SLOT7_START_ADDR 	0x8060240
#define SLOT8_START_ADDR 	0x80602A0
#define SLOT9_START_ADDR 	0x8060300
#define SLOT10_START_ADDR 	0x8060360
#define SLOT11_START_ADDR 	0x80603C0
#define SLOT12_START_ADDR 	0x8060420
#define SLOT13_START_ADDR 	0x8060480
#define SLOT14_START_ADDR 	0x80604E0
#define SLOT15_START_ADDR 	0x8060540
#define SLOT16_START_ADDR 	0x80605A0
*/

char FLASH_Program(session_typedef * pslot, unsigned char slot);
void saveData(char * pData, char fin, uint32_t addInicial);
void saveData2(unsigned int * pMemory, session_typedef * pslot, unsigned int start_addr, unsigned char slot);
void checkData(char * pData, char fin, uint32_t addInicial);
void checkData2(unsigned int * pData, uint32_t addInicial);
unsigned char readDataFromFlash(session_typedef * pslot, unsigned char slot);

#endif
