#include "palette.h"
#include <cstring>
#include <cfloat>

CachedPalette::CachedPalette() {colors = NULL; nlevels = 0;}

CachedPalette::CachedPalette(int nlevels) {
  colors = new Color [nlevels];
  this->nlevels = nlevels;
}

CachedPalette::CachedPalette(const CachedPalette& pal) {
  colors = NULL;
  *this = pal;
}

CachedPalette::~CachedPalette() {
  if (colors) delete [] colors;
}

CachedPalette& CachedPalette::operator=(const CachedPalette& pal) {
  if (nlevels != pal.nlevels) {
    if (colors) delete [] colors;    
    nlevels = pal.nlevels;
    colors = new Color [nlevels];
  }
  memcpy(colors, pal.colors, nlevels * sizeof(Color));
  return *this;
}

Palette::Color CachedPalette::inUnit(float v) const {
  int ind = (int)(v * nlevels);
  if (ind < 0) ind = 0;
  else if (ind >= nlevels) ind = nlevels - 1;

  return colors[ind];
}

CachedPalette PaletteGenerator::cache(int levels) const {
  CachedPalette pal(levels);

  float sc = 1.0 / (levels - 1);
  for (int i = 0; i < levels; i++)
    pal[i] = inUnit(i * sc);

  return pal;
}

LinearPalette::LinearPalette() { }

LinearPalette::LinearPalette(const Palette::Color& c0, const Palette::Color& c1) {
  source = CachedPalette(2);
  source[0] = c0;
  source[1] = c1;
}

LinearPalette::LinearPalette(int ncolors) {
  source = CachedPalette(ncolors);
}

Palette::Color LinearPalette::inUnit(float v) const {
  const float max = 1.0 - FLT_EPSILON;

  if (v < 0.0) v = 0.0;
  else if (v > max) v = max;
  v *= colors() - 1;

  int ind = (int)v;
  v -= ind;

  Color c = source[ind], c1 = source[ind + 1];

  c.r = ByteImage::interp(c.r, c1.r, v);
  c.g = ByteImage::interp(c.g, c1.g, v);
  c.b = ByteImage::interp(c.b, c1.b, v);

  return c;
}

LinearPalette jetPalette() {
  LinearPalette pal(9);
  pal[0] = Palette::Color(0x00, 0x00, 0x7F);
  pal[1] = Palette::Color(0x00, 0x00, 0xFF);
  pal[2] = Palette::Color(0x00, 0x7F, 0xFF);
  pal[3] = Palette::Color(0x00, 0xFF, 0xFF);
  pal[4] = Palette::Color(0x7F, 0xFF, 0x7F);
  pal[5] = Palette::Color(0xFF, 0xFF, 0x00);
  pal[6] = Palette::Color(0xFF, 0x7F, 0x00);
  pal[7] = Palette::Color(0xFF, 0x00, 0x00);
  pal[8] = Palette::Color(0x7F, 0x00, 0x00);
  return pal;
}
