#include <byteimage/template.h>
#include <byteimage/byteimage_sdl2.h>
#include <cstdio>

int main(int argc, char* argv[]) {
  ByteImage img(256, 256, 1);
  
  int x, y, w, h;
  for (int rect = 0; rect < 30; rect++) {
    x = rand() % 256;
    y = rand() % 256;
    w = rand() % (256 - x);
    h = rand() % (256 - y);

    for (int r = 0; r < h; r++)
      for (int c = 0; c < w; c++)
	img.at(r + y, c + x) = !img.at(r + y, c + x) * 255;
  }

  Template T(img, 16, 1);
  T.centerAt(128, 128);
  for (int r, c; T.next(r, c);)
    img.at(r, c) = !img.at(r + y, c + x) * 255;

  display(img);
    
  return 0;
}
