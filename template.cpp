/*
 * template.cpp by Brian Jackson
 * Revised 15 May 2014
 */
#include "template.h"

Template::Template(int size, bool isCircular) {
  img = NULL;
  if (isCircular) setCircular(size);
  else setBox(size);
}

Template::Template(const ByteImage& img, int size, bool isCircular) {
  setImage(img); 
  if (isCircular) setCircular(size);
  else setBox(size);
}

void Template::setImage(const ByteImage& img) {
  this->img = &img;
  rewind();
}

void Template::setBox(int size) {
  Pt p;

  points.clear();
  for (p.r = -size; p.r <= size; p.r++)
    for (p.c = -size; p.c <= size; p.c++)
      points.push_back(p);
      
  width = 2 * size + 1;
  rewind();
}

void Template::setCircular(int size) {
  Pt p;

  points.clear();
  for (p.r = -size; p.r <= size; p.r++)
    for (p.c = -size; p.c <= size; p.c++)
      if (p.r * p.r + p.c * p.c <= size * size)
	points.push_back(p);
      
  width = 2 * size + 1;
  rewind();
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
