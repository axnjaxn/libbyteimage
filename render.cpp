#include "render.h"
#include <cstring>

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}
inline float fabs(float f) {return (f >= 0)? f : -f;}

void DrawRect(ByteImage& target, int x, int y, int w, int h,
	      ByteImage::BYTE v) {
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (x + w > target.nc)
    w = target.nc - x;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > target.nr)
    h = target.nr - y;  

  for (int ch = 0; ch < target.nchannels; ch++)
    for (int r = y; r < y + h; r++)
      memset(target.pixels + ch * target.nr * target.nc + r * target.nc + x, v, w);
}

void DrawRect(ByteImage& target, int x, int y, int w, int h,
	      ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B) {
if (x < 0) {
    w += x;
    x = 0;
  }
  if (x + w > target.nc)
    w = target.nc - x;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > target.nr)
    h = target.nr - y;  

  for (int r = y; r < y + h; r++) {
    memset(target.R() + r * target.nc + x, R, w);
    memset(target.G() + r * target.nc + x, G, w);
    memset(target.B() + r * target.nc + x, B, w);
  }
}

void DrawPoint(ByteImage& target, int x, int y,
	       ByteImage::BYTE v, int sz) {
  x = x - (sz >> 1);
  y = y - (sz >> 1);
  int w = sz, h = sz;
  DrawRect(target, x, y, w, h, v);  
}

void DrawPoint(ByteImage& target, int x, int y,
	       ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz) {
  x = x - (sz >> 1);
  y = y - (sz >> 1);
  int w = sz, h = sz;
  DrawRect(target, x, y, w, h, R, G, B);
}

void DrawLine(ByteImage& target, int ax, int ay, int bx, int by,
	      ByteImage::BYTE v, int sz) {
  float slope, tx, ty;

  if (fabs(by - ay) <= fabs(bx - ax)) {
    slope = (float)(by - ay) / (bx - ax);    
    if (ax > bx) {
      swap(ax, bx);
      swap(ay, by);
    }
      

    tx = ax; ty = ay;
    while (tx <= bx) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), v, sz);
      tx++;
      ty += slope;
    }
  }
  else {
    slope = (float)(bx - ax) / (by - ay);
    if (ay > by) {
      swap(ax, bx);
      swap(ay, by);
    }

    tx = ax; ty = ay;
    while (ty <= by) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), v, sz);
      tx += slope;
      ty++;
    }
  }
}

void DrawLine(ByteImage& target, int ax, int ay, int bx, int by,
	      ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz) {
  float slope, tx, ty;

  if (fabs(by - ay) <= fabs(bx - ax)) {
    slope = (float)(by - ay) / (bx - ax);    
    if (ax > bx) {
      swap(ax, bx);
      swap(ay, by);
    }

    tx = ax; ty = ay;
    while (tx <= bx) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), R, G, B, sz);
      tx++;
      ty += slope;
    }
  }
  else {
    slope = (float)(bx - ax) / (by - ay);
    if (ay > by) {
      swap(ax, bx);
      swap(ay, by);
    }

    tx = ax; ty = ay;
    while (ty <= by) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), R, G, B, sz);
      tx += slope;
      ty++;
    }
  }
}
