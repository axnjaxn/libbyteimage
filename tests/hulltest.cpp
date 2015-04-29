#include <byteimage/component.h>
#include <byteimage/byteimage_sdl2.h>
#include <cstdio>

using namespace byteimage;

class HullDisplay : public Display {
protected:
  ByteImage img;
  BitImage bimg;
  ByteImage::BYTE threshold = 0x80;
  int minsize = 10;

  void art(bool inverse = false) {
    ByteImage canvas(img.nr, img.nc, 3);
    ByteImage I = std::move(img);

    if (inverse)
      for (int i = 0; i < I.size(); i++) I[i] = 255 - I[i];

    for (int ch = 0; ch < 3; ch++) {
      img = I.selectChannel(ch);
      threshold = 0xFF;
      frameDelay = 0;
      do {
	threshold += 4;
	bimg = BitImage::threshold(img, threshold);
	doProcess();
	for (int r = 0; r < img.nr; r++)
	  for (int c = 0; c < img.nc; c++)
	    if (bimg.at(r, c))
	      canvas.at(r, c, ch) = threshold;
	if (show(canvas)) exit(0);
      } while (threshold != 0xFF);
    }
    
    if (inverse) {
      for (int i = 0; i < I.size(); i++) I[i] = 255 - I[i];
      for (int i = 0; i < canvas.size(); i++) canvas[i] = 255 - canvas[i];
    }

    updateImage(canvas);
    img = std::move(I);
  }

  void doProcess() {
    std::vector<Component> comps = Component::getComponents(bimg);
    bimg.clear();
    for (int i = 0; i < comps.size(); i++) {
      if (comps[i].size() > minsize) {
	comps[i].fillHull();
	for (auto pt : comps[i].points)
	  bimg.set(pt.r, pt.c);
      }
    }
  }

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN)
      switch (event.key.keysym.sym) {
      case SDLK_a:
	art();
	break;
      case SDLK_i:
	art(true);
	break;
      case SDLK_RETURN:
	doProcess();
	updateImage(bimg.toByteImage());
	break;
      case SDLK_UP:
	if (threshold < 255 - 4) 
	  threshold += 4;
	bimg = BitImage::threshold(img, threshold);
	updateImage(bimg.toByteImage());
	break;
      case SDLK_DOWN:
	if (threshold > 4)
	  threshold -= 4;
	bimg = BitImage::threshold(img, threshold);
	updateImage(bimg.toByteImage());
	break;
      }
    Display::handleEvent(event);
  }

public:
  HullDisplay(const ByteImage& img) : Display(img), img(img) {
    bimg = BitImage::threshold(img, threshold);
    updateImage(bimg.toByteImage());
    frameDelay = 100;
  }
};

int main(int argc, char* argv[]) {
  if (argc == 1) return 0;

  ByteImage img(argv[1]);

  while (img.nr > 800 || img.nc > 1200) img = img.d2();
  HullDisplay(img).main();

  return 0;
}
