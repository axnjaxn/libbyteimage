#include <byteimage/bytevideo.h>
#include <byteimage/byteimage_sdl2.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  ByteImage img(256, 256);

  {
    ByteVideoWriter writer("test.avi", 256, 256);
    for (int frame = 0; frame < 240; frame++) {
      for (int c = 0; c < 256; c++)
	img.at(0, c) = ((c + frame) % 10)? 0 : 255;
      
      for (int r = 1; r < 256; r++)
	memcpy(img.pixels + r * 256, img.pixels, 256);
      
      printf("\rWriting frame %d", frame + 1);
      fflush(0);
      
      writer.write(img.toColor());
    }
  }
  
  printf("\nWrote test.avi\n");

  ByteVideo video("test.avi"); 
  printf("Video reports %d frames, with size %d x %d and %lf fps\n", video.count(), video.width(), video.height(), video.FPS());
  video.nextFrame(img);

  ByteImageDisplay disp(img, "Video output");
  disp.frameDelay = 0;

  Uint32 ticks = SDL_GetTicks(), dt;
  double delay = 1000.0 / video.FPS();
  while (video.nextFrame(img)) {
    if (disp.show(img)) break;
    dt = SDL_GetTicks() - ticks;
    ticks += dt;
    if (dt < delay)
      SDL_Delay(delay - dt);
  }

  return 0;
}
