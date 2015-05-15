#ifndef _BPJ_BYTEIMAGE_TEMPLATE_H
#define _BPJ_BYTEIMAGE_TEMPLATE_H

#include "byteimage.h"
#include <vector>

namespace byteimage {
  class Template {
  protected:
    //TODO: Deprecated
    typedef byteimage::Pt Pt;

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

    void getCenter(int& r, int& c);
    void centerAt(int r, int c);//Shifts template center
    void rewind();//Resets state
    bool next(int& r, int& c);//Next r, c in image bounds; returns false if none left
  };
}

#endif
