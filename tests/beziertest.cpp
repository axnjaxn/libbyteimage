#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <ctime>
#include <cstdlib>

using namespace byteimage;

int main(int argc, char* argv[]) {
  srand(time(NULL));

  ByteImage canvas(384, 512, 3);

  double step = 0.001;

  Matrix a = makePoint(64, 383), b = makePoint(128, 128), c = makePoint(384, 128), d = makePoint(448, 383);
  Matrix e, f, g, h, i, j;
  double t = 0.0;
  
  std::vector<Matrix> path;
  path.push_back(a);
  path.push_back(b);
  path.push_back(c);
  path.push_back(d);
  DrawBezier(canvas, path, makeColor(64, 64 ,64));

  Display disp(canvas);
  disp.frameDelay = 10;

  ByteImage img(canvas);
  Matrix color = makeColor(255, 0, 0);
  Matrix white = makeColor(255, 255, 255);
  
  while (!disp.show(img)) {
    img = canvas;
    DrawLine(img, a, b, white);
    DrawLine(img, b, c, white);
    DrawLine(img, c, d, white);
    
    e = (1.0 - t) * a + t * b;
    f = (1.0 - t) * b + t * c;
    g = (1.0 - t) * c + t * d;
    DrawLine(img, e, f, white);
    DrawLine(img, f, g, white);
    DrawPoint(img, e, white, 3);
    DrawPoint(img, f, white, 3);
    DrawPoint(img, g, white, 3);


    h = (1.0 - t) * e + t * f;
    i = (1.0 - t) * f + t * g;
    DrawLine(img, h, i, white);
    DrawPoint(img, h, white, 3);
    DrawPoint(img, i, white, 3);

    j = (1.0 - t) * h + t * i;


    DrawPoint(canvas, j, color, 3);
    DrawPoint(img, j, color, 5);

    t += step;
    if (t > 1.0 && step > 0) {
      step = -step;
      ByteImage::BYTE R, G, B;
      hsl2rgb(rand() % 360, 1.0, 0.5, R, G, B);
      color = makeColor(R, G, B);
    }
    if (t < 0.0 && step < 0) {
      step = -step;
      ByteImage::BYTE R, G, B;
      hsl2rgb(rand() % 360, 1.0, 0.5, R, G, B);
      color = makeColor(R, G, B);
    }
  }  
}
