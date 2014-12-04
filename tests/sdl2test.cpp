#include <byteimage/byteimage_sdl2.h>

class ImageFlicker : public ByteImageDisplay {
protected:
  void update() {
    for (int i = 0 ; i < img.size(); i++)
      img[i] = !img[i] * 255;
    updateImage(img);
    ByteImageDisplay::update();
  }

public:
  ImageFlicker(const ByteImage& img) : ByteImageDisplay(img, "ByteImage SDL2 Test") {
    this->img = img;
    frameDelay = 500;
  }

  ByteImage img;
};

int main(int argc, char* argv[]) {
  ByteImage img(256, 256);
  for (int r = 0; r < img.nr; r++)
    for (int c = 0; c < img.nc; c++)
      img.at(r, c) = (((r / 16) + (c / 8)) & 1)? 0 : 255;

  ImageFlicker(img).main();

  return 0;
}
