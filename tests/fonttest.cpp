#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <byteimage/font.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  ByteImage img(96, 600, 3);
  
  //Draw background gradient
  double t;
  for (int c = 0; c < img.nc; c++) {
    t = (double)c / (img.nc - 1); 
    img.at(0, c, 2) = ByteImage::clip(255.0 * t);
  }
  for (int r = 1; r < img.nr; r++) {
    memcpy(img.R() + r * img.nc, img.R(), img.nc);
    memcpy(img.G() + r * img.nc, img.G(), img.nc);
    memcpy(img.B() + r * img.nc, img.B(), img.nc);
  }

  const char* str = "dolphins abduct by proxy the ever awkward klutz";

  //Render font
  TextRenderer text("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 24);

  text.drawUnkerned(img, str, img.nr - 60, 0, 255, 0, 0);
  
  int x, y, w, h;
  text.getBox(str, x, y, w, h);
  DrawRect(img, x, y + img.nr - 36, w, h, 0, 0, 255);
  text.draw(img, str, img.nr - 36, 0, 255, 192, 0);

  text.drawUnkerned(img, str, img.nr - 12, 0, 255, 0, 0);
  text.draw(img, str, img.nr - 12, 0, 255, 192, 0);
  
  display(img);

  return 0;
}
