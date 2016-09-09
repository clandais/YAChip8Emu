/*
 * Graphics.h
 *
 *  Created on: 27 ao�t 2016
 *      Author: cleme
 */

#ifndef SRC_INCLUDE_GRAPHICS_H_
#define SRC_INCLUDE_GRAPHICS_H_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>


unsigned char GetKeyDown(void);

int IsKeyPressed(char keycode);

char WaitForKeyPress();

void Run(int cyclesPerSeconds);

#endif /* SRC_INCLUDE_GRAPHICS_H_ */
