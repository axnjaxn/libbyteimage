/*
 * template.h by Brian Jackson
 * Revised 15 May 2014
 */

#ifndef _BPJ_BYTEIMAGE_TEMPLATE_H
#define _BPJ_BYTEIMAGE_TEMPLATE_H

#include "byteimage.h"
#include <vector>

class Template {
protected:
  class Pt {
  public:
    int r, c;
    Pt() {r = c = 0;}
    Pt(int r, int c) {this->r = r; this->c = c;}
  };

  const ByteImage* img;
  std::vector<Pt> points;
  Pt center;
  int width, nextIndex;

public:
  Template(int size = 0, bool isCircular = 0);
  Template(const ByteImage& img, int size = 0, bool isCircular = 0);

  inline int getWidth() const {return width;}

  void setImage(const ByteImage& img);
  const ByteImage& getImage() const {return *img;}

  void setBox(int size);//Resets set to rectangular template with width 2 * size + 1
  void setCircular(int size);//Resets set to circular template with diameter 2 * size + 1

  void centerAt(int r, int c);//Shifts template center
  void rewind();//Resets state
  bool next(int&r, int& c);//Next r, c in image bounds; returns false if none left
};

#endif
