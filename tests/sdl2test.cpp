#include <byteimage/byteimage_sdl2.h>

int main(int argc, char* argv[]) {
  ByteImage img(256, 256, 3);
  for (int i = 0; i < img.size(); i++)
    img[i] = (i & 1)? 0 : 255;

  display(img);

  return 0;
}
