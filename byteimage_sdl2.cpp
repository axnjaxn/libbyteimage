/*
 * byteimage_sdl2
 * Updated 10 May 2014
 * Written by Brian Jackson
 */

#include "byteimage_sdl2.h"

SDL_Texture* toTexture(SDL_Renderer* renderer, const ByteImage& img) {
  if (img.nchannels == 1) return toTexture(renderer, img.toColor());
  SDL_Texture* texture = SDL_CreateTexture(renderer,
					   SDL_PIXELFORMAT_ARGB8888,
					   SDL_TEXTUREACCESS_STATIC,
					   img.nc, img.nr);
  
  //Is this something that can be done in place?
  Uint32* px = new Uint32 [img.nr * img.nc];
  const ByteImage::BYTE *r = img.R(), *g = img.G(), *b = img.B();
  for (int i = 0; i < img.nr * img.nc; i++)
    px[i] = 0xFF000000 | (r[i] << 16) | (g[i] << 8) | b[i];
  SDL_UpdateTexture(texture, NULL, px, img.nc * sizeof (Uint32));

  delete [] px;
  return texture;
}

void display(const ByteImage& img) {
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("Error: Could not initialize SDL.\n");
      return;
    }
    atexit(SDL_Quit);
  }
  
  SDL_Window* window = SDL_CreateWindow("Image Display",
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					img.nc, img.nr,
					0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture* texture = toTexture(renderer, img);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  bool exitflag = 0;
  SDL_Event event;
  while (!exitflag) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
	if (event.window.windowID == SDL_GetWindowID(window))
	  exitflag = 1;
      }
      else if (event.type == SDL_QUIT) exitflag = 1;
    }

    SDL_Delay(100);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
}
