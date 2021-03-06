#include <byteimage/template.h>
#include <byteimage/byteimage_sdl2.h>
#include <cstdio>

using namespace byteimage;

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

  Template T = Template::makeSerialCircle(16);
  T.setImage(img);
  T.centerAt(128, 128);
  for (int r, c; T.next(r, c);)
    img.at(r, c) = !img.at(r, c) * 255;

  img = img.toColor();
  T = Template::makeCircle(16);
  T.setImage(img);
  T.centerAt(128, 128);
  for (int r, c, i = 0; T.next(r, c); i++)
    img.at(r, c, 0) = (i / 5) & 0xFF;

  Display(img).main();
    
  return 0;
}
