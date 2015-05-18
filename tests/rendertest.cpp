#include <byteimage/render.h>
#include <byteimage/byteimage_sdl2.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  ByteImage img(256, 256, 3);
  DrawLine(img, 0, 0, 255, 255, 255); //White line from top left to bottom right
  DrawLine(img, Pt2f(255, 0), Pt2f(0, 255), Color(255, 255, 0), 3); //Yellow line, thickness 3, from top right to bottom left
  DrawPoint(img, 128, 128, 0, 255, 255, 9); //Great big cyan point in the middle
  DrawRect(img, 192, 0, 300, 255, 128); //Gray partially-offscreen rectangle
  DrawRect(img, 0, 192, 255, 300, 64); //Dark Gray partially-offscreen rectangle
  DrawRect(img, 0, 0, 4, 0, 0, 255, 0);//Zero-height green rectangle
  DrawRect(img, 0, 0, 0, 4, 255, 0, 255);//Zero-width magenta rectangle
  
  std::vector<Pt2f> path;
  path.push_back(Pt2f(0, 255));
  path.push_back(Pt2f(0, 0));
  path.push_back(Pt2f(255, 0));
  path.push_back(Pt2f(255, 255));
  DrawBezier(img, path, Color(192, 0, 0), 5);

  DrawTriangle(img, Pt2f(10, 10), Pt2f(30, 20), Pt2f(0, 40), Color(0, 0, 255));
  DrawTriangle(img, Pt2f(10, 50), Pt2f(30, 20), Pt2f(0, 40), Color(0, 255, 0));

  Display(img).main();  
  
  return 0;
}
