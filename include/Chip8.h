
#ifndef SRC_INCLUDE_CHIP8_C_
#define SRC_INCLUDE_CHIP8_C_

#include "Graphics.h"
unsigned char* GetGfx();
char GetDrawFlag();

unsigned char GetRegisters(int index);
unsigned short GetStack(int index);
unsigned short GetOpcode();


void ChipEmulateCycle(void);

void ChipLoadRomInMemory(char filePath[255]);

void ChipInitialize();

void ChipEnd();

void Chip8UpdateTimers(void);


#endif
