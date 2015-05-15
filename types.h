#ifndef _BPJ_BYTEIMAGE_TYPES_H
#define _BPJ_BYTEIMAGE_TYPES_H

namespace byteimage {
  typedef unsigned char Byte;
  
  constexpr Byte clip(float f) {return (f > 255)? 255 : (f < 0)? 0 : (Byte)(f + 0.5);}
  constexpr Byte avg(Byte a, Byte b) {return (a >> 1) + (b >> 1) + ((a & 1) & (b & 1));}
  constexpr Byte diff(Byte a, Byte b) {return (a > b)? a - b : b - a;}
  constexpr Byte interp(Byte a, Byte b, float t) {return (t < 0)? a : (t > 1)? b : clip((1 - t) * a + t * b);}

  template <typename tn> class Grid;//TODO
  template <typename tn> class Volume;//TODO

  class Color {
  public:
    Byte r, g, b;
    constexpr Color() : r(0), g(0), b(0) { }
    constexpr Color(Byte r, Byte g, Byte b) : r(r), g(g), b(b) { }
  };
  void rgb2hsl(Byte r, Byte g, Byte b, float &h, float &s, float &l);
  void hsl2rgb(float h, float s, float l, Byte &r, Byte &g, Byte &b);

  class Pt {
  public:
    int r, c;
    constexpr Pt() : r(0), c(0) { }
    constexpr Pt(int r, int c) : r(r), c(c) { }
  };  
  
  class Pt2f;//TODO
  class Pt3f;//TODO
}

#endif
