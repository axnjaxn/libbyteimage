#include "types.h"

using namespace byteimage;

void byteimage::rgb2hsl(Byte r, Byte g, Byte b, float &h, float &s, float &l) {
  Byte min, max;
  float c;

  max = (r > g)? r : g;
  max = (max > b)? max : b;

  min = (r < g)? r : g;
  min = (min < b)? min : b;
  
  c = max - min;

  //Find hue in degrees
  if (c == 0) h = 0;
  else if (max == r) {
    h = (g - b) / c;
    if (b > g) h += 6.0;
  }
  else if (max == g) h = (b - r) / c + 2;
  else h = (r - g) / c + 4;

  h = 60 * h;
  
  //Find lightness
  const float d255 = 3.921568627e-3;
  const float d510 = 0.5 * d255;
  l = (max + min) * d510;
  
  //Find saturation
  if (l > 0.0 && l < 1.0) {
    s = c * d255;
    if (l > 0.5) s /= 2 - 2 * l;
    else s /= 2 * l;
  }
  else
    s = 0.0;
}

void byteimage::hsl2rgb(float h, float s, float l, Byte &R, Byte &G, Byte &B) {
  float c = (l <= 0.5)? c = 2 * l * s : (2 - 2 * l) * s;

  const float d60 = 1.0 / 60.0;
  h = h * d60;
  int htype = (int)h;
  h  = (htype & 1) + (h - htype);//h = fmod(h, 2)
  h = (h > 1)? h - 1 : 1 - h;
  float x = c * (1 - h);

  float r, g, b;
  switch (htype) {
  default: r = 0; g = 0; b = 0; break;
  case 0: r = c; g = x; b = 0; break;
  case 1: r = x; g = c; b = 0; break;
  case 2: r = 0; g = c; b = x; break;
  case 3: r = 0; g = x; b = c; break;
  case 4: r = x; g = 0; b = c; break;
  case 5: r = c; g = 0; b = x; break;
  }    

  x = l - 0.5 * c;
  R = clip(255.0f * (r + x));
  G = clip(255.0f * (g + x));
  B = clip(255.0f * (b + x));
}
