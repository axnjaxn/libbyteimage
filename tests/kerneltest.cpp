#include <byteimage/kernel.h>
#include <byteimage/byteimage_sdl2.h>
#include <cstdio>

using namespace byteimage;

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

  ByteImage result(256 * 2, 256 * 3, 1);
  result.blit(img, 0, 0);
  
  Kernel K = Kernel::Gaussian(2.0);
  printf("Gaussian with sigma = 2.0\n");
  printf("Min: %lf\n", K.min());
  printf("Max: %lf\n", K.max());
  result.blit(K.convolve(K.transpose().convolve(img)), 0, 256);
  result.blit(K.convolveSeparable(img), 0, 512);

  K = Kernel::Gradient(2.0);
  printf("Gradient with sigma = 2.0\n");
  printf("Min: %lf\n", K.min());
  printf("Max: %lf\n", K.max());
  result.blit(K.convolveMagnitude(img), 256, 0);

  K = Kernel::LoG(2.0);
  printf("LoG with sigma = 2.0\n");
  printf("Min: %lf\n", K.min());
  printf("Max: %lf\n", K.max());
  result.blit(K.convolveMagnitude(img), 256, 256);
  
  K = Kernel::LoG2D(2.0);
  result.blit(K.convolve(img), 256, 512);

  Display(result).main();
    
  return 0;
}
