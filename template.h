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
    bool operator<(const Pt& p) const {return r * r + c * c < p.r * p.r + p.c * p.c;}
  };

  const ByteImage* img;
  std::vector<Pt> points;
  Pt center;
  int width, nextIndex;

public:
  Template();

  inline int getWidth() const {return width;}

  void setImage(const ByteImage& img);
  const ByteImage& getImage() const {return *img;}

  static Template makeBox(int radius); //Rectangular template with width (2 * radius + 1), prioritizing center locations
  static Template makeSerialBox(int radius); //Rectangular template with width (2 * radius + 1), with increasing index
  static Template makeCircle(int radius); //Circular template with width (2 * radius + 1), prioritizing center locations
  static Template makeSerialCircle(int radius); //Circular template with width (2 * radius + 1), with increasing index
  
  void centerAt(int r, int c);//Shifts template center
  void rewind();//Resets state
  bool next(int&r, int& c);//Next r, c in image bounds; returns false if none left
};

#endif
