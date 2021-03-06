#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <ctime>
#include <cstdlib>

using namespace byteimage;

int main(int argc, char* argv[]) {
  srand(time(NULL));

  ByteImage canvas(384, 512, 3);

  float step = 0.001;

  Pt2f a = Pt2f(64, 383), b = Pt2f(128, 128), c = Pt2f(384, 128), d = Pt2f(448, 383);
  Pt2f e, f, g, h, i, j;
  float t = 0.0;
  
  std::vector<Pt2f> path;
  path.push_back(a);
  path.push_back(b);
  path.push_back(c);
  path.push_back(d);
  DrawBezier(canvas, path, Color(64, 64 ,64));

  Display disp(canvas);
  disp.frameDelay = 10;

  ByteImage img(canvas);
  Color color(255, 0, 0);
  Color white(255, 255, 255);
  
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
      Byte R, G, B;
      hsl2rgb(rand() % 360, 1.0, 0.5, R, G, B);
      color = Color(R, G, B);
    }
    if (t < 0.0 && step < 0) {
      step = -step;
      Byte R, G, B;
      hsl2rgb(rand() % 360, 1.0, 0.5, R, G, B);
      color = Color(R, G, B);
    }
  }  
}
