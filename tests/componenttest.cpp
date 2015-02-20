#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <byteimage/font.h>
#include <byteimage/component.h>

int main(int argc, char* argv[]) {
  ByteImage img(128, 600, 3);
  ByteImageDisplay disp(img);
  
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

  disp.show(img);
  
  BitImage b = BitImage::threshold(img, 96);
  b.open();
  disp.show(b.toByteImage());

  img = ByteImage(img.nr, img.nc, 3);

  Uint32 ticks = SDL_GetTicks();
  for (int i = 1; i < 1000; i++)
    std::vector<Component> components = Component::getComponents(b);
  std::vector<Component> components = Component::getComponents(b);
  ticks = SDL_GetTicks() - ticks;
  printf("Finished after %dms\n", ticks);

  ByteImage::BYTE R, G, B;
  for (int i = 0; i < components.size(); i++) {
    R = (rand() & 0xBF) | 0x40;
    G = (rand() & 0xBF) | 0x40;
    B = (rand() & 0xBF) | 0x40;
    for (int j = 0, r, c; j < components[i].size(); j++) {
      r = components[i][j].r;
      c = components[i][j].c;
      img.at(r, c, 0) = R;
      img.at(r, c, 1) = G;
      img.at(r, c, 2) = B;
    }
  }

  disp.show(img);
  disp.main();

  return 0;
}
