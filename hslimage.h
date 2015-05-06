#ifndef _BPJ_BYTEIMAGE_HSLIMAGE_H
#define _BPJ_BYTEIMAGE_HSLIMAGE_H

#include "byteimage.h"

namespace byteimage {
  class HSLImage {
  public:
    float* hsl;//Stored H, S, L interlaced
    int nr, nc;

  public:
    HSLImage();
    HSLImage(int nr, int nc);
    HSLImage(const ByteImage& img);
    HSLImage(const HSLImage& img);
    HSLImage(HSLImage&& img);
    virtual ~HSLImage();

    HSLImage& operator=(const HSLImage& img);
    HSLImage& operator=(HSLImage&& img);

    inline float& H(int r, int c) {return hsl[3 * (r * nc + c)];}
    inline float H(int r, int c) const {return hsl[3 * (r * nc + c)];}
    inline float& S(int r, int c) {return hsl[3 * (r * nc + c) + 1];}
    inline float S(int r, int c) const {return hsl[3 * (r * nc + c) + 1];}
    inline float& L(int r, int c) {return hsl[3 * (r * nc + c) + 2];}
    inline float L(int r, int c) const {return hsl[3 * (r * nc + c) + 2];}

    ByteImage toHue();
    ByteImage toSaturation() const;
    ByteImage toLightness() const;
    ByteImage toByteImage() const;

    void panSharpen(const ByteImage& gray);
    HSLImage panSharpened(const ByteImage& gray) const;
  };

  float hueDistance(float h0, float h1);
}

#endif
