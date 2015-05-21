#include <byteimage/byteimage_sdl2.h>
#include <byteimage/bitimage.h>
#include <byteimage/render.h>
#include <byteimage/font.h>

using namespace byteimage;

void drawStroked(const TextRenderer& font,
		 ByteImage& target, const char* str, int r, int c,
		 Color fill, Color stroke = Color()) {
  int x, y, w, h;
  font.getBox(str, x, y, w, h);

  ByteImage temp(h + 2, w + 2);
  font.draw(temp, str, 1 - y, 1 - x, 255);

  BitImage mask(temp.nr, temp.nc);
  for (int r = 0, i = 0; r < mask.nr; r++)
    for (int c = 0; c < mask.nc; c++, i++)
      if (temp[i] > 128) mask.set(r, c);
      else temp[i] = 0;
    
  mask.dilate();

  Byte *R = target.R(), *G = target.G(), *B = target.B();
  
  for (int y1 = 0; y1 < mask.nr; y1++)
    for (int x1 = 0; x1 < mask.nc; x1++) {
      w = (y + y1 + r) * target.nc + (x + x1 + c);
      if (temp.at(y1, x1)) {
	R[w] = temp.at(y1, x1) * fill.r / 255;
	G[w] = temp.at(y1, x1) * fill.g / 255;
	B[w] = temp.at(y1, x1) * fill.b / 255;
      }
      else if (mask.at(y1, x1)) {
	R[w] = stroke.r;
	G[w] = stroke.g;
	B[w] = stroke.b;
      }
    }
}

int main(int argc, char* argv[]) {
  ByteImage img(96, 600, 3);
  
  //Draw background gradient
  double t;
  for (int c = 0; c < img.nc; c++) {
    t = (double)c / (img.nc - 1); 
    img.at(0, c, 2) = clip(255.0 * t);
  }
  for (int r = 1; r < img.nr; r++) {
    memcpy(img.R() + r * img.nc, img.R(), img.nc);
    memcpy(img.G() + r * img.nc, img.G(), img.nc);
    memcpy(img.B() + r * img.nc, img.B(), img.nc);
  }

  const char* str = "dolphins abduct by proxy the ever awkward klutz";

  //Render font
  TextRenderer text("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 24);
  
  drawStroked(text, img, str, img.nr - 60, 0, Color(255, 0, 0), Color(255));
  
  int x, y, w, h;
  text.getBox(str, x, y, w, h);
  DrawRect(img, x, y + img.nr - 36, w, h, 0, 0, 255);
  text.draw(img, str, img.nr - 36, 0, 255, 192, 0);

  text.drawUnkerned(img, str, img.nr - 12, 0, 255, 0, 0);
  text.draw(img, str, img.nr - 12, 0, 255, 192, 0);
  
  Display(img).main();

  return 0;
}
