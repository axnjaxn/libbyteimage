#include <byteimage/kernel.h>
#include <byteimage/byteimage_sdl2.h>
#include <cstdio>

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

  Kernel K = Kernel::Gaussian(3.0);

  ByteImage G = K.convolve(K.transpose().convolve(img));

  ByteImage result(256, 512, 1);
  result.blit(img, 0, 0);
  result.blit(G, 0, 256);

  display(result);
    
  return 0;
}
