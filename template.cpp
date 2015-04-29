#include "template.h"
#include <algorithm>

using namespace byteimage;

Template::Template() : img(nullptr), width(0), nextIndex(0) { }

void Template::setImage(const ByteImage& img) {
  this->img = &img;
  rewind();
}

Template Template::makeBox(int radius) {
  Template T = makeSerialBox(radius);
  std::stable_sort(T.points.begin(), T.points.end());
  return T;
}

Template Template::makeSerialBox(int radius) {
  Template T;
  Pt p;

  for (p.r = -radius; p.r <= radius; p.r++)
    for (p.c = -radius; p.c <= radius; p.c++)
      T.points.push_back(p);
      
  T.width = 2 * radius + 1;

  return T;
}

Template Template::makeCircle(int radius) {
  Template T = makeSerialCircle(radius);
  std::stable_sort(T.points.begin(), T.points.end());
  return T;
}

Template Template::makeSerialCircle(int radius) {
  Template T;
  Pt p;

  for (p.r = -radius; p.r <= radius; p.r++)
    for (p.c = -radius; p.c <= radius; p.c++)
      if (p.r * p.r + p.c * p.c <= radius * radius)
	T.points.push_back(p);
      
  T.width = 2 * radius + 1;
  
  return T;
}

void Template::centerAt(int r, int c) {
  center = Pt(r, c);
  rewind();
}

void Template::rewind() {
  nextIndex = 0;
}

bool Template::next(int&r, int& c) {
  for (; nextIndex < points.size(); nextIndex++) {
    r = center.r + points[nextIndex].r;
    c = center.c + points[nextIndex].c;
    if (!img || img->inBounds(r, c)) {
      nextIndex++;
      return 1;
    }
  }
  return 0;
}
