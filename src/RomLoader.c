
#include <stdlib.h>
#include <stdio.h>

#include "RomLoader.h"


void GetRomData(char filePath[255], unsigned char memory[4096])
{

	int i;
	unsigned char res;

	FILE *file;
	file = fopen(filePath, "rb");
	if (!file)
	{
		printf("Unable to open file %s\n", filePath);
		exit(-2);
	}

	i = 0;

	while (!feof(file))
	{
		fread(&res, sizeof(char), 1, file);
		memory[i + 0x200] = res;
		i++;
	}

	fclose(file);
}
