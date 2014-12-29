#include <byteimage/byteimage_sdl2.h>
#include <byteimage/font.h>

int main(int argc, char* argv[]) {
  ByteImage img(300, 400, 3);
  
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

  //Render font
  TextRenderer text("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 36);
  text.draw(img, "Mind your ps and qs", img.nr - 1, 0, 255, 192, 0);

  display(img);

  return 0;
}
