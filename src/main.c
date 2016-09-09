
#include "Chip8.h"

#include <string.h>
#include <stdio.h>

int main(int argc, char* args[] )
{
	int i;
	char filePath[1024];


	if( argc > 1)
	{
		strcpy(filePath, "");
		
		for(i = 1; i < argc; i++)
		{
			strcat(filePath, args[i]);
		}
	}

	ChipInitialize();
	ChipLoadRomInMemory(filePath);

	Run(1000 / 500);


	return 0;
}
