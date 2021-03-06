#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <byteimage/palette.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  Display disp(512, 512, "Palette Test");

  CachedPalette pal = LinearPalette::jet().cache(16);
  
  ByteImage canvas(512, 512, 3);
  Color color;
  double v;
  for (int i = 0; i < canvas.nc; i++) {
    v = (i - 256.0) / 128.0;
    color = pal.inRange(v);
    DrawRect(canvas, i, 0, 1, 512, color.r, color.g, color.b);
  }

  disp.show(canvas);
  disp.main();

  return 0;
}
