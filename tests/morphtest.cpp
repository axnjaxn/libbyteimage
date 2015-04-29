#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <byteimage/font.h>
#include <byteimage/bitimage.h>

using namespace byteimage;

class MorphDisplay : public ByteImageDisplay {
protected:
  BitImage img;

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_e: img.erode(); break;
      case SDLK_d: img.dilate(); break;
      case SDLK_o: img.open(); break;
      case SDLK_c: img.close(); break;
      case SDLK_i: img.invert(); break;
      }
      updateImage(img.toByteImage());
    }
    ByteImageDisplay::handleEvent(event);
  }
  
public:
  MorphDisplay(const BitImage& img) : ByteImageDisplay(img.toByteImage()) {
    this->img = img;
  }
};

int main(int argc, char* argv[]) {
  ByteImage img(128, 600, 3);
  
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
  
  BitImage b = BitImage::threshold(img, 80);
  BitImage b2 = BitImage::threshold(img, 96);

  b.XOR(b2);

  MorphDisplay(b).main();

  return 0;
}
