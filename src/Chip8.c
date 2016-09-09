
#include "Chip8.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Chip8Tables.h"
#include "Graphics.h"
#include "RomLoader.h"


/*
memory starts at 0x200 (512),
bytes below are reserved for the computer
*/
unsigned char memory[4096];
/*
The 16 data registers
*/
unsigned char V[16];

unsigned char GetRegisters(int index)
{
	return V[index];
}

/*
Program counter. Increments by 2 each cycle
Starts at 0x200
*/
unsigned short PC;

/*
GFX, aligned pixels
*/
unsigned char gfx[64 * 32];
unsigned char* GetGfx()
{
	return gfx;
}

/*
I: the I register
*/
unsigned short I;

/*
The stack stores memory adresses from where subroutines are called
*/
unsigned short stack[16];

unsigned short GetStack(int index)
{
	return stack[index];
}

int stackPointer;


char drawFlag = 0;
/*
 * Return the draw flag;
 * if it is set to true, sets it false
 */
char GetDrawFlag()
{
	if(drawFlag)
	{
		drawFlag = 0;
		return 1;
	}
	else
	{
		return drawFlag;
	}
}

int delay_timer = 0;
int sound_timer = 0;

void Chip8UpdateTimers(void)
{
	if (delay_timer > 0)
		delay_timer--;
	if (sound_timer > 0)
		sound_timer--;
}


void FetchOpcode(void);


unsigned char fontset[80] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

unsigned short opcode = 0x0000;

unsigned short GetOpcode()
{
	return opcode;
}


void ChipInitialize()
{
	int i;

	srand(time(NULL));

	PC = 0x200;
	stackPointer = 0;

	/* load fonset at beginning of memory */

	for (i = 0; i < 80; i++)
	{
		memory[i] = fontset[i];
	}

}


void ChipLoadRomInMemory(char filePath[255])
{
	GetRomData(filePath, memory);
}


void ChipEnd()
{

}


void ChipEmulateCycle(void)
{

	FetchOpcode();

	Chip8Table[(opcode & 0xF000) >> 12]();

}

void FetchOpcode()
{
	opcode = memory[PC] << 8 | (memory[PC + 1]);
	PC += 2;
}





/*==========================================

	OPCODES

============================================*/


void CpuNull()
{
	printf("Unimplemented opcode %X\n", opcode);
}


void OpcodeZero()
{
	if ((opcode == 0x00E0 || opcode == 0x00EE)) Chip8Zero[(opcode & 0x000F)]();
}


/*
	1NNN	Jump to address NNN
*/
void OpcodeOne(void)
{
	unsigned short NNN = opcode & 0x0FFF;
	PC = NNN;
}

/*
	2NNN	Execute subroutine starting at address NNN
*/
void OpcodeTwo(void)
{
	stack[stackPointer] = PC;
	stackPointer++;
	PC = opcode & 0x0FFF;
}

/*
	3XNN	Skip the following instruction if the value of register VX equals NN
*/
void OpcodeThree(void)
{
	unsigned short X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;

	if (V[X] == NN) PC += 2;
}


/*
	4XNN	Skip the following instruction if the value of register VX is not equal to NN
*/
void OpcodeFour(void)
{
	unsigned short X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;

	if (V[X] != NN) PC += 2;
}


/*
	Wikipedia:
		5XY0	Skips the next instruction if VX equals VY.
*/
void OpcodeFive(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	if (V[X] == V[Y]) PC += 2;
}


/*
	6XNN	Store number NN in register VX
*/
void OpcodeSix(void)
{
	unsigned short X = (opcode & 0x0F00) >> 8;
	unsigned char NN = opcode & 0x00FF;

	V[X] = NN;
}

/*
	7XNN	Add the value NN to register VX
*/
void OpcodeSeven(void)
{
	unsigned char NN = opcode & 0x00FF;
	unsigned short X = (opcode & 0x0F00) >> 8;
	V[X] += NN;
}

void OpcodeHeight(void)
{
	Chip8Arithmetic[opcode & 0x000F]();
}


/*
	9XY0
		Skip the following instruction if the value of register VX is not equal to the value of register VY
*/
void OpcodeNine(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;

	if (V[X] != V[Y]) PC += 2;
}


/*
	ANNN
		Store memory address NNN in register I
*/
void OpcodeA(void)
{
	I = opcode & 0x0FFF;
}


/*
	Wikipedia:
		CXNN	Sets VX to the result of a bitwise and operation on a random number and NN.
	mattmik:
		CXNN	Set VX to a random number with a mask of NN
*/
void OpcodeC(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned short NN = opcode & 0x00FF;

	V[X] = (rand() % 0xFF) & NN;
}

/*
	DXYN
		Draw a sprite at position VX, VY with N bytes of sprite data
		starting at the address stored in I
		Set VF to 01 if any set pixels are changed to unset, and 00 otherwise
*/
void OpcodeD(void)
{
	unsigned char X = V[(opcode & 0x0F00) >> 8];
	unsigned char Y = V[(opcode & 0x00F0) >> 4];
	unsigned char numBytes = opcode & 0x000F;
	int yLine, xLine;

	for (yLine = 0; yLine < numBytes; yLine++)
	{
		for (xLine = 0; xLine < 8; xLine++)
		{

			if ( (memory[I + yLine] & (0x80 >> xLine)) != 0)
			{
				if (gfx[(X + xLine + ((Y + yLine) * 64))] == 1)
					V[0xF] = 1;
				else
					V[0xF] = 0;
				gfx[X + xLine + ((Y + yLine) * 64)] ^= 1;
			}
		}
	}

	drawFlag = 1;

}



void OpcodeE(void)
{
	switch (opcode & 0x00FF)
	{
	/*
		Wikipedia:
			EX9E	Skips the next instruction if the key stored in VX is pressed.
		mattmik:
			EX9E	Skip the following instruction if the key corresponding to the hex value currently stored in register VX is pressed
	*/
	case 0x9E:
	{
		unsigned char X = (opcode & 0x0F00) >> 8;
		if (GetKeyDown() == V[X])
			PC += 2;

	} break;

	/*
		Wikipedia:
			EXA1	Skips the next instruction if the key stored in VX isn't pressed.
	*/
	case 0xA1:
	{
		unsigned char X = (opcode & 0x0F00) >> 8;
		if (GetKeyDown() != V[X])
			PC += 2;
	} break;

	default:
		CpuNull();
		break;
	}
}


void OpcodeF(void)
{
	Chip8F[opcode & 0x000F]();
}


/*
	00E0	Clear the screen
*/
void Opcode00E0(void)
{
	int i;
	for (i = 0; i < 64 * 32; i++) gfx[i] = 0;
	drawFlag = 1;
}

/*
	00EE	Return from a subroutine
*/
void Opcode00EE(void)
{
	stackPointer--;
	PC = stack[stackPointer];
}


/*
	8XY0	Store the value of register VY in register VX
*/
void Opcode8XY0(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[Y];
}

/*
	Wikipedia:
		8XY1	Sets VX to VX or VY.
*/
void Opcode8XY1(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] | V[Y];
}

/*
	Wikipedia:
		8XY2	Sets VX to VX and VY.
*/

void Opcode8XY2(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] & V[Y];
}

/*
	Wikipedia:
		8XY3	Sets VX to VX xor VY.
*/
void Opcode8XY3(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] = V[X] ^ V[Y];
}
/*

	wikipedia:
		8XY4
			Adds VY to VX.
			VF is set to 1 when there's a carry, and to 0 when there isn't.
	mattmik:
		8XY4
			Add the value of register VY to register VX
			Set VF to 01 if a carry occurs
			Set VF to 00 if a carry does not occur
*/

void Opcode8XY4(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;

	if ( V[Y] > (0xFF - V[X]) ) /* carry */
		V[0xF] = 0x1;
	else
		V[0xF] = 0x0;
	V[X] += V[Y];
}

/*
	Wikipedia:
		8XY5
			VY is subtracted from VX.
			VF is set to 0 when there's a borrow, and 1 when there isn't.
	mattmik:
		8XY5
			Subtract the value of register VY from register VX
			Set VF to 00 if a borrow occurs
			Set VF to 01 if a borrow does not occur


	Borrow ex: adding 1 to char c = 255 == 0

*/
void Opcode8XY5(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;

	if (V[X] < V[Y]) /* borrow */
		V[0xF] = 0x0;
	else
		V[0xF] = 0x1;

	V[X] -= V[Y];

}

/*
	8XY6	Store the value of register VY shifted right one bit in register VX
			Set register VF to the least significant bit prior to the shift
			^^ NOT WORKING

			Wikipedia:
				Shifts VX right by one.
				VF is set to the value of the least significant bit of VX before the shift.[2]

*/
void Opcode8XY6(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;

	V[X] >>= 1;
	V[0xF] = V[Y] & 0x1;
}


/*
	Wikipedia:
		8XY7
			Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
*/
void Opcode8XY7(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;

	if (V[Y] < V[X]) /* borrow */
		V[0xF] = 0;
	else
		V[0xF] = 1;
	V[X] = V[Y] - V[X];
}

/*
	8XYE	Store the value of register VY shifted left one bit in register VX
			Set register VF to the most significant bit prior to the shift
			^^ NOT WORKING

			Wikipedia:
				Shifts VX left by one.
				VF is set to the value of the most significant bit of VX before the shift.[2]

*/
void Opcode8XYE(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	unsigned char Y = (opcode & 0x00F0) >> 4;
	V[X] <<= 1;
	V[0xF] = V[Y] >> 7;
}


void OpcodeFXN5(void)
{
	switch (opcode & 0x00FF)
	{

	/*
		WIkipedia:
			FX15
				Sets the delay timer to VX.
	*/
	case 0x15:
	{
		unsigned char X = (opcode & 0x0F00) >> 8;
		delay_timer = V[X];

	} break;


	/*
		Wikipedia:
			FX55
				Stores V0 to VX (including VX) in memory starting at address I.[4]
	*/
	case 0x55:
	{
		unsigned char X = (opcode & 0x0F00) >> 8;
		int i;

		for (i = 0; i <= X; i++)
		{
			memory[I + i] = V[i];
		}

	} break;

	/*
		FX65	Fill registers V0 to VX inclusive with the values stored in memory starting at address I
				I is set to I + X + 1 after operation
	*/
	case 0x65:
	{
		unsigned char X = (opcode & 0x0F00) >> 8;
		unsigned char i;
		for (i = 0; i <= X; i++)
		{
			V[i] = memory[I + i];
		}
		I = I + X + 1;

	} break;

	default:
		CpuNull();
		break;
	}
}

/*
	Wikipedia:
		FX07
			Sets VX to the value of the delay timer.
*/
void OpcodeFX07(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	V[X] = (char)delay_timer;
}


/*
	Wikipedia:
		FX18	Sets the sound timer to VX.
*/
void OpcodeFX18(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	sound_timer = V[X];
}


/*
	Wikipedia
		FX0A
			A key press is awaited, and then stored in VX.
*/
void OpcodeFX0A(void)
{
	unsigned short X = (opcode & 0x0F00) >> 8;
	V[X] = WaitForKeyPress();
}

/*
	Wikipedia:
	FX29
		Sets I to the location of the sprite for the character in VX.
		Characters 0-F (in hexadecimal) are represented by a 4x5 font.

	mattmik
	FX29
		Set I to the memory address of the sprite data corresponding
		to the hexadecimal digit stored in register VX
*/
void OpcodeFX29(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	I = V[X] * 0x5;
}



/*
	FX1E	Add the value stored in register VX to register I
*/
void OpcodeFX1E(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	I += V[X];
}



/*
	Wikipedia:
		FX33
			Stores the binary-coded decimal representation of VX,
			with the most significant of three digits at the address in I,
			the middle digit at I plus 1, and the least significant digit at I plus 2.

			(In other words, take the decimal representation of VX, place the hundreds digit in memory
			at location in I,
			the tens digit at location I+1, and the ones digit at location I+2.)
*/
void OpcodeFX33(void)
{
	unsigned char X = (opcode & 0x0F00) >> 8;
	memory[I] = V[X] / 100;
	memory[I + 1]	= V[X] % 100 / 10;
	memory[I + 2]	= V[X] % 10;
}
