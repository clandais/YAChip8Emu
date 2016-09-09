
#ifndef SRC_INCLUDE_CHIP8TABLES_H_
#define SRC_INCLUDE_CHIP8TABLES_H_


void(*Chip8Table[17])(void);
void(*Chip8Arithmetic[16])(void);
void(*Chip8Zero[15])(void);
void(*Chip8F[16])(void);

void OpcodeZero(void);
void Opcode00E0(void);
void Opcode00EE(void);

void OpcodeOne(void);

void OpcodeTwo(void);

void OpcodeThree(void);

void OpcodeFour(void);

void OpcodeFive(void);

void OpcodeSix(void);

void OpcodeSeven(void);

void OpcodeHeight(void);
void Opcode8XY0(void);
void Opcode8XY1(void);
void Opcode8XY2(void);
void Opcode8XY3(void);
void Opcode8XY4(void);
void Opcode8XY5(void);
void Opcode8XY6(void);
void Opcode8XY7(void);
void Opcode8XYE(void);

void OpcodeNine(void);

void OpcodeA(void);

void OpcodeC(void);

void OpcodeD(void);

void OpcodeE(void);

void OpcodeF(void);

void OpcodeFX07(void);
void OpcodeFX0A(void);
void OpcodeFX18(void);
void OpcodeFX1E(void);
void OpcodeFX29(void);
void OpcodeFX33(void);

/*
 * FX15, FX55, FX65
 */
void OpcodeFXN5(void);

void CpuNull(void);


void(*Chip8Table[17])(void) =

{
	OpcodeZero, OpcodeOne, OpcodeTwo, OpcodeThree, OpcodeFour, OpcodeFive,
	OpcodeSix, OpcodeSeven, OpcodeHeight, OpcodeNine, OpcodeA, CpuNull,
	OpcodeC, OpcodeD, OpcodeE, OpcodeF, CpuNull
};

void(*Chip8Zero[15])(void) =
{
	Opcode00E0,	 CpuNull, CpuNull,
	CpuNull, CpuNull, CpuNull,
	CpuNull, CpuNull, CpuNull,
	CpuNull, CpuNull, CpuNull,
	CpuNull, CpuNull, Opcode00EE
};


void(*Chip8Arithmetic[16])(void) =
{
	Opcode8XY0, Opcode8XY1, Opcode8XY2,
	Opcode8XY3, Opcode8XY4, Opcode8XY5,
	Opcode8XY6, Opcode8XY7, CpuNull,
	CpuNull, CpuNull, CpuNull,
	CpuNull, CpuNull, Opcode8XYE,
	CpuNull
};

void(*Chip8F[16])(void) =
{
	CpuNull, CpuNull, CpuNull,
	OpcodeFX33, CpuNull, OpcodeFXN5,
	CpuNull, OpcodeFX07, OpcodeFX18,
	OpcodeFX29, OpcodeFX0A, CpuNull,
	CpuNull, CpuNull, OpcodeFX1E,
	CpuNull
};

#endif
