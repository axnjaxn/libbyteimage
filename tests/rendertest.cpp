#include <byteimage/render.h>
#include <byteimage/byteimage_sdl2.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  ByteImage img(256, 256, 3);
  DrawLine(img, 0, 0, 255, 255, 255); //White line from top left to bottom right
  DrawLine(img, makePoint(255, 0), makePoint(0, 255), makeColor(255, 255, 0), 3); //Yellow line, thickness 3, from top right to bottom left
  DrawPoint(img, 128, 128, 0, 255, 255, 9); //Great big cyan point in the middle
  DrawRect(img, 192, 0, 300, 255, 128); //Gray partially-offscreen rectangle
  DrawRect(img, 0, 192, 255, 300, 64); //Dark Gray partially-offscreen rectangle
  DrawRect(img, 0, 0, 4, 0, 0, 255, 0);//Zero-height green rectangle
  DrawRect(img, 0, 0, 0, 4, 255, 0, 255);//Zero-width magenta rectangle
  
  std::vector<Matrix> path;
  path.push_back(makePoint(0, 255));
  path.push_back(makePoint(0, 0));
  path.push_back(makePoint(255, 0));
  path.push_back(makePoint(255, 255));
  DrawBezier(img, path, makeColor(192, 0, 0), 5);

  DrawTriangle(img, makePoint(10, 10), makePoint(30, 20), makePoint(0, 40), makeColor(0, 0, 255));
  DrawTriangle(img, makePoint(10, 50), makePoint(30, 20), makePoint(0, 40), makeColor(0, 255, 0));

  ByteImageDisplay(img).main();  
  
  return 0;
}
