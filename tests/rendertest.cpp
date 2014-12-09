#include <byteimage/render.h>
#include <byteimage/byteimage_sdl2.h>

int main(int argc, char* argv[]) {
  ByteImage img(256, 256, 3);
  DrawLine(img, 0, 0, 255, 255, 255); //White line from top left to bottom right
  DrawLine(img, 255, 0, 0, 255, 255, 255, 0, 3); //Yellow line, thickness 3, from top right to bottom left
  DrawPoint(img, 128, 128, 0, 255, 255, 9); //Great big cyan point in the middle
  DrawRect(img, 192, 0, 300, 255, 128); //Gray partially-offscreen rectangle
  DrawRect(img, 0, 192, 255, 300, 64); //Dark Gray partially-offscreen rectangle
  DrawRect(img, 0, 0, 4, 0, 0, 255, 0);//Zero-height green rectangle
  DrawRect(img, 0, 0, 0, 4, 255, 0, 255);//Zero-width magenta rectangle
  
  ByteImageDisplay(img).main();  
  
  return 0;
}
