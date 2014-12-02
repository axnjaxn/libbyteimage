#include <byteimage/bytevideo.h>
#include <byteimage/byteimage_sdl2.h>

int main(int argc, char* argv[]) {
  ByteImage img(256, 256);

  ByteVideoWriter writer("test.avi", 256, 256);
  for (int frame = 0; frame < 240; frame++) {
    for (int c = 0; c < 256; c++)
      img.at(0, c) = ((c + frame) % 10)? 0 : 255;
    
    for (int r = 1; r < 256; r++)
      memcpy(img.pixels + r * 256, img.pixels, 256);
    
    printf("\rWriting frame %d", frame + 1);
    fflush(0);

    writer.write(img);
  }
  writer.close();
  printf("\nWrote test.avi\n");

  ByteVideo video("test.avi"); 
  printf("Video reports %d frames\n", video.count());
  video.nextFrame(img);
  SDL_Window* window = SDL_CreateWindow("Video output",
					SDL_WINDOWPOS_CENTERED,
					SDL_WINDOWPOS_CENTERED,
					img.nc, img.nr,
					0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture* texture = toTexture(renderer, img);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_Event event;
  Uint32* px = new Uint32 [img.nr * img.nc];
  while (video.nextFrame(img)) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE) {
	if (event.window.windowID == SDL_GetWindowID(window))
	  exit(0);
      }
      else if (event.type == SDL_QUIT) exit(0);
    }

    const ByteImage::BYTE *r = img.R(), *g = img.G(), *b = img.B();
    for (int i = 0; i < img.nr * img.nc; i++)
      px[i] = 0xFF000000 | (r[i] << 16) | (g[i] << 8) | b[i];
    SDL_UpdateTexture(texture, NULL, px, img.nc * sizeof (Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_Delay(5);
  }
  
  delete [] px;

  return 0;
}
