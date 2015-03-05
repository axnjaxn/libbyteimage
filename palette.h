#ifndef _BPJ_BYTEIMAGE_PALETTE_H
#define _BPJ_BYTEIMAGE_PALETTE_H

#include "byteimage.h"

class Palette {
public:
  class Color {
  public:
    ByteImage::BYTE r, g, b;
    inline Color() {r = g = b = 0;}
    inline Color(ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b) {this->r = r; this->g = g; this->b = b;}
  };
  
  //These functions *must* ensure range safety.
  inline virtual Color inRange(float v) const {return inUnit(0.5 * (v + 1));}//in [-1, 1]
  virtual Color inUnit(float v) const = 0;//in [0, 1]
};

//This is a nearest neighbor
class CachedPalette : public Palette {
protected:
  Color* colors;
  int nlevels;

public:
  CachedPalette();
  CachedPalette(int nlevels);
  CachedPalette(const CachedPalette& pal);
  virtual ~CachedPalette();

  CachedPalette& operator=(const CachedPalette& pal);
  inline Color& operator[](int i) {return colors[i];}
  inline const Color& operator[](int i) const {return colors[i];}

  inline int levels() const {return nlevels;}

  virtual Color inUnit(float v) const;
};

class PaletteGenerator : public Palette {
public:
  CachedPalette cache(int levels) const;
};

class LinearPalette : public PaletteGenerator {
protected:
  CachedPalette source;
  
public:
  LinearPalette();
  LinearPalette(const Color& c0, const Color& c1);
  LinearPalette(int ncolors);

  inline Color& operator[](int i) {return source[i];}
  inline const Color& operator[](int i) const {return source[i];}

  inline int colors() const {return source.levels();}

  virtual Color inUnit(float v) const;
};

LinearPalette jetPalette();

#endif
