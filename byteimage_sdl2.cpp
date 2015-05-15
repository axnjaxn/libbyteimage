#include "byteimage_sdl2.h"

using namespace byteimage;

Display::Display(int nr, int nc, const char* title) {init(ByteImage(nr, nc), title);}

Display::Display(const ByteImage& img, const char* title) {init(img, title);}

Display::Display(Display&& disp) 
  : window(disp.window), texture(disp.texture), renderer(disp.renderer), 
    px(disp.px), exitflag(disp.exitflag), drawflag(disp.drawflag), frameDelay(disp.frameDelay)  {
  disp.window = nullptr;
  disp.texture = nullptr;
  disp.renderer = nullptr;
  disp.px = nullptr;
}

Display::~Display() {
  if (window) SDL_DestroyWindow(window);
  if (texture) SDL_DestroyTexture(texture);
  if (renderer) SDL_DestroyRenderer(renderer);
  delete [] px;
}

void Display::init(const ByteImage& img, const char* title) {
  if (!SDL_WasInit(SDL_INIT_VIDEO)) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      printf("Error: Could not initialize SDL.\n");
      return;
    }
    atexit(SDL_Quit);
  }

  exitflag = 1;
  drawflag = 0;
  frameDelay = 5;
  
  window = SDL_CreateWindow(title? title : "Image Display",
			    SDL_WINDOWPOS_CENTERED,
			    SDL_WINDOWPOS_CENTERED,
			    img.nc, img.nr,
			    0);
  renderer = SDL_CreateRenderer(window, -1, 0);
  texture = SDL_CreateTexture(renderer,
			      SDL_PIXELFORMAT_ARGB8888,
			      SDL_TEXTUREACCESS_STATIC,
			      img.nc, img.nr);

  px = new Uint32 [img.nr * img.nc];
  updateImage(img);
}

void Display::handleEvent(SDL_Event event) {
  if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
    if (event.window.windowID == SDL_GetWindowID(window))
      exitflag = 1;
  }
  else if (event.type == SDL_QUIT) exitflag = 1;
}

void Display::updateImage(const ByteImage& img) {
  writePixelArray(px, img);
  SDL_UpdateTexture(texture, NULL, px, img.nc * sizeof (Uint32));
  drawflag = 1;
}

void Display::update() {
  if (drawflag) {
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    drawflag = 0;
  }

  if (frameDelay)
    SDL_Delay(frameDelay);
}

void Display::main() {
  exitflag = 0;
  SDL_Event event;
  while (!exitflag) {
    while (SDL_PollEvent(&event))
      handleEvent(event);

    update();
  }
}

int Display::show(const ByteImage& img) {
  exitflag = 0;
  
  SDL_Event event;
  while (SDL_PollEvent(&event))
    handleEvent(event);

  updateImage(img);
  update();
  
  return exitflag;
}

void byteimage::writePixelArray(Uint32* dest, const ByteImage& img) {
  const Byte *r, *g, *b;

  if (img.nchannels == 3) {
    r = img.R();
    g = img.G();
    b = img.B();
  }
  else r = g = b = img.pixels;

  for (int i = 0; i < img.nr * img.nc; i++)
    dest[i] = 0xFF000000 | (r[i] << 16) | (g[i] << 8) | b[i];
}

SDL_Texture* byteimage::toTexture(SDL_Renderer* renderer, const ByteImage& img) {
  if (img.nchannels == 1) return toTexture(renderer, img.toColor());
  SDL_Texture* texture = SDL_CreateTexture(renderer,
					   SDL_PIXELFORMAT_ARGB8888,
					   SDL_TEXTUREACCESS_STATIC,
					   img.nc, img.nr);
  
  Uint32* px = new Uint32 [img.nr * img.nc];
  writePixelArray(px, img);
  SDL_UpdateTexture(texture, NULL, px, img.nc * sizeof (Uint32));

  delete [] px;
  return texture;
}
