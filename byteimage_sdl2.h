/*
 * byteimage_sdl2
 * Updated 10 May 2014
 * Written by Brian Jackson
 */

#ifndef _BPJ_BYTEIMAGE_SDL2_H
#define _BPJ_BYTEIMAGE_SDL2_H

#include "byteimage.h"
#include <SDL2/SDL.h>

SDL_Texture* toTexture(SDL_Renderer* renderer, const ByteImage& img);
void display(const ByteImage& img);

#endif
