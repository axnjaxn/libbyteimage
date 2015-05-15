#ifndef _BPJ_BYTEIMAGE_TYPES_H
#define _BPJ_BYTEIMAGE_TYPES_H

#include <cmath>

namespace byteimage {
  typedef unsigned char Byte;
  
  constexpr Byte clip(float f) {return (f > 255)? 255 : (f < 0)? 0 : (Byte)(f + 0.5);}
  constexpr Byte avg(Byte a, Byte b) {return (a >> 1) + (b >> 1) + ((a & 1) & (b & 1));}
  constexpr Byte diff(Byte a, Byte b) {return (a > b)? a - b : b - a;}
  constexpr Byte interp(Byte a, Byte b, float t) {return (t < 0)? a : (t > 1)? b : clip((1 - t) * a + t * b);}

  class Color {
  public:
    Byte r, g, b;
    constexpr Color() : r(0), g(0), b(0) { }
    constexpr Color(Byte v) : r(v), g(v), b(v) { }
    constexpr Color(Byte r, Byte g, Byte b) : r(r), g(g), b(b) { }
  };

  class Pt {
  public:
    int r, c;
    constexpr Pt() : r(0), c(0) { }
    constexpr Pt(int r, int c) : r(r), c(c) { }
  };  

  class Pt2f {
  public:
    float x, y;
    
    constexpr Pt2f(float x = 0.0, float y = 0.0) : x(x), y(y) { }
    constexpr Pt2f operator+(const Pt2f& a) const {return Pt2f(x + a.x, y + a.y);}
    constexpr Pt2f operator-(const Pt2f& a) const {return Pt2f(x - a.x, y - a.y);}
    constexpr Pt2f operator*(float f) const {return Pt2f(x * f, y * f);}
    constexpr Pt2f operator/(float f) const {return Pt2f(x / f, y / f);}
    constexpr friend Pt2f operator*(float f, const Pt2f& pt) {return pt * f;}
    
    constexpr float sqLength() const {return x * x + y * y;}
    inline float length() const {return sqrt(sqLength());}
    constexpr friend float dot(const Pt2f& a, const Pt2f& b) {return a.x * b.x + a.y * b.y;}
  };

  class Pt3f {
  public:
    float x, y, z;
    
    constexpr Pt3f(float x = 0.0, float y = 0.0, float z = 0.0) : x(x), y(y), z(z) { }
    constexpr Pt3f operator+(const Pt3f& a) const {return Pt3f(x + a.x, y + a.y, z + a.z);}
    constexpr Pt3f operator-(const Pt3f& a) const {return Pt3f(x - a.x, y - a.y, z - a.z);}
    constexpr Pt3f operator*(float f) const {return Pt3f(x * f, y * f, z * f);}
    constexpr Pt3f operator/(float f) const {return Pt3f(x / f, y / f, z / f);}
    constexpr friend Pt3f operator*(float f, const Pt3f& pt) {return pt * f;}
    
    constexpr float sqLength() const {return x * x + y * y + z * z;}
    inline float length() const {return sqrt(sqLength());}
    constexpr friend float dot(const Pt3f& a, const Pt3f& b) {return a.x * b.x + a.y * b.y + a.z * b.z;}
  };
}

#endif
