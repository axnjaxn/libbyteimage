#ifndef _BPJ_BYTEIMAGE_PALETTE_H
#define _BPJ_BYTEIMAGE_PALETTE_H

#include "byteimage.h"

namespace byteimage {
  class Palette {
  public:
    class Color {
    public:
      ByteImage::BYTE r, g, b;
      constexpr Color() : r(0), g(0), b(0) { }
      constexpr Color(ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b) : r(r), g(g), b(b) { }
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
    CachedPalette(CachedPalette&& pal);
    virtual ~CachedPalette();

    static CachedPalette fromBytes(std::initializer_list<int> v);
    static CachedPalette fromHex(std::initializer_list<int> v);
    static CachedPalette fromFloats(std::initializer_list<float> v);
    static CachedPalette fromColors(std::initializer_list<Palette::Color> v);

    CachedPalette& operator=(const CachedPalette& pal);
    CachedPalette& operator=(CachedPalette&& pal);
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
    LinearPalette(const CachedPalette& pal);
    LinearPalette(int ncolors);

    inline Color& operator[](int i) {return source[i];}
    inline const Color& operator[](int i) const {return source[i];}

    inline int colors() const {return source.levels();}

    virtual Color inUnit(float v) const;

    static LinearPalette hue();
    static LinearPalette jet();
    static LinearPalette parula();
  };
}

#endif
