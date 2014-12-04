/*
 * byteimage_sdl2
 * Updated 4 December 2014
 * Written by Brian Jackson
 */

#ifndef _BPJ_BYTEIMAGE_SDL2_H
#define _BPJ_BYTEIMAGE_SDL2_H

#include "byteimage.h"
#include <SDL2/SDL.h>

//An extensible image viewer
class ByteImageDisplay {
protected:
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  Uint32* px;
  bool exitflag, drawflag;
  
  void init(const ByteImage& img, const char* title);

  /*
   * These methods can all be overridden.
   * In general, you'll want to call the parent 
   * member function last in the overrides.
   * If you want to suppress the default 
   * texture-overwrites-display behavior,
   * set drawflag = 0 in your update function.
   */
  virtual void handleEvent(SDL_Event event);      //Called per event to respond to UI events
  virtual void updateImage(const ByteImage& img); //Call this to update the background texture and set draw flag
  virtual void update();                          //Override this for per-frame behavior. It is responsible for redraw and timing.

public:
  unsigned int frameDelay;

  ByteImageDisplay(int nr, int nc, const char* title = NULL);
  ByteImageDisplay(const ByteImage& img, const char* title = NULL);
  virtual ~ByteImageDisplay();

  void main();
};

void writePixelArray(Uint32* dest, const ByteImage& img);
SDL_Texture* toTexture(SDL_Renderer* renderer, const ByteImage& img);
void display(const ByteImage& img);

#endif
