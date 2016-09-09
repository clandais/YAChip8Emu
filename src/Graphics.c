/*
 * Graphics.c
 *
 *  Created on: 27 ao�t 2016
 *      Author: cleme
 */


#include "Graphics.h"
#include "Chip8.h"

void DrawGfx();
Uint32 timerCallback( Uint32 interval, void* param );

SDL_Window *window;
SDL_Surface *surface;
SDL_Renderer *renderer;

SDL_TimerID cycleTimer, delayTimer;
Uint32 cyclesFrequency;
int quit;


/*
 * Creates video related stuff
 *
 */
void InitVideo()
{

	/*
	 *
	 Uint32 rmask, gmask, bmask, amask;
	*/
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to init Video %s \n", SDL_GetError());
		exit(-1);
	}

	window = SDL_CreateWindow("Chip 8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
	if(!window)
	{
		printf("Failed to create window %s \n", SDL_GetError());
		exit(-2);
	}

	surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_RenderSetScale(renderer, 10, 10);


	SDL_UpdateWindowSurface(window);

}

/*
 * Free video related stuff
 */
void DestroyVideo()
{
	SDL_RemoveTimer(cycleTimer);
	SDL_RemoveTimer(delayTimer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}



unsigned char keyDown = 255;
unsigned char GetKeyDown(void)
{
	return keyDown;
}

void Run(int cyclesPerSeconds)
{

	Uint32 sixtyHertz = 1000 / 60;
	SDL_Event event;

	InitVideo();

	cyclesFrequency = (Uint32) cyclesPerSeconds;

	cycleTimer = SDL_AddTimer(cyclesFrequency, timerCallback, "cycle");
	delayTimer = SDL_AddTimer(sixtyHertz, timerCallback, "delay");

	quit = 0;
	while(!quit)
	{

		if(GetDrawFlag()) DrawGfx();

		while(SDL_PollEvent(&event) != 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = 1;
				break;

			case SDL_USEREVENT:
				if( strcmp(event.user.data1, "cycle") == 0)
					ChipEmulateCycle();
				else if( strcmp(event.user.data1, "delay") == 0)
					Chip8UpdateTimers();

				break;

			default:
				break;
			}

		}

		SDL_Delay(1); /* Avoids heavy CPU use */
	}

	DestroyVideo();
}

Uint32 timerCallback( Uint32 interval, void* param )
{
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;

    if 		(strcmp( param, "cycle" ) == 0) userevent.data1 = "cycle";
    else if (strcmp( param, "delay" ) == 0) userevent.data1 = "delay";
    else 									userevent.data1 = "";

    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return (interval);
}

/*
 * Gets the gfx array from the chip and draw each pixels
 */
void DrawGfx()
{
	unsigned char* gfx = GetGfx();
	int x, y;

	for (x = 0; x < 64; x++)
		for (y = 0; y < 32; y++)
		{
			if (gfx[y * 64 + x] == 1)
			{
				SDL_SetRenderDrawColor(renderer, 0 , 255, 0, 255);
				SDL_RenderDrawPoint(renderer, x, y);
			}
			else
			{
				SDL_SetRenderDrawColor(renderer, 0 , 0, 0, 255);
				SDL_RenderDrawPoint(renderer, x, y);
			}
		}

	SDL_RenderPresent(renderer);

}

char KeyPressEventProcessing(SDL_Event event)
{
	char key = 0x11;
	/*
	Keypad                   Keyboard
	+-+-+-+-+                +-+-+-+-+
	|1|2|3|C|                |1|2|3|4|
	+-+-+-+-+                +-+-+-+-+
	|4|5|6|D|                |Q|W|E|R|
	+-+-+-+-+       =>       +-+-+-+-+
	|7|8|9|E|                |A|S|D|F|
	+-+-+-+-+                +-+-+-+-+
	|A|0|B|F|                |Z|X|C|V|
	+-+-+-+-+                +-+-+-+-+
	*/
	if (event.type == SDL_KEYDOWN)
	{
		switch (event.key.keysym.sym)
		{

		case SDLK_1:
			key = 0x1;
			break;
		case SDLK_2:
			key =  0x2;
			break;
		case SDLK_3:
			key =  0x3;
			break;
		case SDLK_4:
			key =  0xC;
			break;
		case SDLK_a:
			key =  0x4;
			break;
		case SDLK_z:
			key =  0x5;
			break;
		case SDLK_e:
			return 0x6;
		case SDLK_r:
			key =  0xD;
			break;
		case SDLK_q:
			key =  0x7;
			break;
		case SDLK_s:
			key =  0x8;
			break;
		case SDLK_d:
			key =  0x9;
			break;
		case SDLK_f:
			key =  0xE;
			break;
		case SDLK_w:
			key =  0xA;
			break;
		case SDLK_x:
			key =  0x0;
			break;
		case SDLK_c:
			key =  0xB;
			break;
		case SDLK_v:
			key =  0xF;
			break;

		default:
			break;
		}
	}
	return key;
}
char WaitForKeyPress()
{
	char key = 0x11; /* unused key */

	SDL_RemoveTimer(cycleTimer);
	while(key == 0x11)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);

		if (event.type == SDL_KEYDOWN && !event.key.repeat)
		{
			key = KeyPressEventProcessing(event);
		}
		else if(event.type == SDL_QUIT)
		{
			quit = 1;
			break;
		}

	}

	cycleTimer = SDL_AddTimer(cyclesFrequency, timerCallback, "cycle");
	return key;

}
