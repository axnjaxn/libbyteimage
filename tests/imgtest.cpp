#include <byteimage/byteimage.h>
#include <cstdio>

using namespace byteimage;

int main(int argc, char* argv[]) {
#ifndef _BYTEIMAGE_NO_MAGICK
  ByteImage img(256, 256, 3);
  for (int i = 0; i < img.size(); i++)
    img[i] = (i & 1)? 0 : 255;
  img.save_filename("imgtest.png");
  printf("Saved imgtest.png\n");

  img.load_filename("imgtest.png");
  img = img.d2().u2();
  img.save_filename("imgtest2.png");
  printf("Saved imgtest2.png\n");
#else
  printf("ByteImage class does not have ImageMagick (Magick++) extensions.\n");
#endif

  return 0;
}
