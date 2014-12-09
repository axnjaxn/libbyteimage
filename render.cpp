#include "render.h"
#include <cstring>

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}
inline float fabs(float f) {return (f >= 0)? f : -f;}

Matrix makePoint(double x, double y, double w) {
  Matrix v(3, 1);
  v.at(0) = x;
  v.at(1) = y;
  v.at(2) = w;
  return v;
}

Matrix makeColor(double r, double g, double b) {return makePoint(r, g, b);}

inline double getX(const Matrix& v) {return v.at(0) / v.at(2);}
inline double getY(const Matrix& v) {return v.at(1) / v.at(2);}
inline double getValue(const Matrix& rgb) {return (rgb.at(0) + rgb.at(1) + rgb.at(2)) / 3.0;}
inline ByteImage::BYTE getR(const Matrix& rgb) {return ByteImage::clip(rgb.at(0));}
inline ByteImage::BYTE getG(const Matrix& rgb) {return ByteImage::clip(rgb.at(1));}
inline ByteImage::BYTE getB(const Matrix& rgb) {return ByteImage::clip(rgb.at(2));}

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

void DrawRect(ByteImage& target, const Matrix& ul, const Matrix& lr, const Matrix& rgb) {
  int x = (int)(getX(ul) + 0.5);
  int y = (int)(getY(ul) + 0.5);
  int w = (int)(getX(lr) + 0.5);
  int h = (int)(getY(lr) + 0.5);

  if (x > w) swap(x, w);
  if (y > h) swap(y, h);
  w -= x;
  h -= y;

  if (target.nchannels == 1)
    DrawRect(target, x, y, w, h, getValue(rgb));
  else
    DrawRect(target, x, y, w, h, getR(rgb), getG(rgb), getB(rgb));
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

void DrawPoint(ByteImage& target, const Matrix& v, const Matrix& rgb, int sz) {
  int x = (int)(getX(v) + 0.5);
  int y = (int)(getY(v) + 0.5);

  if (target.nchannels == 1)
    DrawPoint(target, x, y, getValue(rgb), sz);
  else
    DrawPoint(target, x, y, getR(rgb), getG(rgb), getB(rgb), sz);
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

void DrawLine(ByteImage& target, const Matrix& a, const Matrix& b, const Matrix& rgb, int sz) {
  int ax = (int)(getX(a) + 0.5);
  int ay = (int)(getY(a) + 0.5);
  int bx = (int)(getX(b) + 0.5);
  int by = (int)(getY(b) + 0.5);

  if (target.nchannels == 1)
    DrawLine(target, ax, ay, bx, by, getValue(rgb), sz);
  else
    DrawLine(target, ax, ay, bx, by, getR(rgb), getG(rgb), getB(rgb), sz);
}

void DrawBezier(ByteImage& target, const std::vector<Matrix>& pts, const Matrix& rgb, int sz, int n) {
  std::vector<Matrix> interp(pts.size());

  Matrix v0 = pts[0], v1;
  double t;
  for (int i = 1; i <= n; i++) {
    t = (double)i / n;
    interp.assign(pts.begin(), pts.end());
    for (int order = pts.size() - 1; order >= 1; order--)
      for (int j = 0; j < order; j++)
	interp[j] = (1.0 - t) * interp[j] + t * interp[j + 1];
    v1 = interp[0];
    DrawLine(target, v0, v1, rgb, sz);
    v0 = v1;
  }
}
