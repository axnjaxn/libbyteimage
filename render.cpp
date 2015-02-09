#include "render.h"
#include <cstring>

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}
inline float fabs(float f) {return (f >= 0)? f : -f;}

Matrix makeColor(double r, double g, double b) {return makePoint(r, g, b);}

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
  if (w <= 0) return;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > target.nr)
    h = target.nr - y;  
  if (h <= 0) return;

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
  if (w <= 0) return;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > target.nr)
    h = target.nr - y;  
  if (h <= 0) return;

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

void DrawCross(ByteImage& target, int x, int y, 
	       ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b,
	       int radius, int line_size) {
  DrawLine(target, x, y - radius, x, y + radius, r, g, b, line_size);
  DrawLine(target, x - radius, y, x + radius, y, r, g, b, line_size);
}

void DrawCross(ByteImage& target, const Matrix& v, const Matrix& rgb, 
	       int radius, int line_size) {
  DrawCross(target, getX(v), getY(v), getR(rgb), getG(rgb), getB(rgb), radius, line_size);
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

void DrawCircle(ByteImage& target, const Matrix& v, const Matrix& rgb, double radius) {
  double r2 = radius * radius;
  int r = (int)(getY(v) + 0.5), c = (int)(getX(v) + 0.5);
  for (int i = (int)(radius + 1); i >= 0; i--)
    for (int j = (int)(radius + 1); j >= 0; j--)
      if (i * i + j * j <= r2) {
	DrawRect(target, c - j, r - i, 2 * j + 1, 1, getR(rgb), getG(rgb), getB(rgb));
	DrawRect(target, c - j, r + i, 2 * j + 1, 1, getR(rgb), getG(rgb), getB(rgb));
      }
}

inline int floor(double d) {return (int)d;}
inline int ceil(double d) {return (int)d + (d != (int)d);}

void DrawTriangle(ByteImage& target, const Matrix& v0, const Matrix& v1, const Matrix& v2, const Matrix& rgb) {
  Matrix u, l, r, d;
  u = v0;
  l = v1;
  d = v2;

  if (getY(l) < getY(u)) swap(u, l);
  if (getY(d) < getY(u)) swap(u, d);
  if (getY(d) < getY(l)) swap(l, d);

  //Form two triangles: ulr and lrd
  r = makePoint(getX(u) + (getY(l) - getY(u))  * (getX(d) - getX(u)) / (getY(d) - getY(u)), getY(l));
  if (getX(l) > getX(r)) swap(l, r);

  float lpos, rpos;
  float lslope, rslope;
  int x, y, w, h = 1;

  //Blit upper triangle
  lpos = rpos = getX(u);
  lslope = (getX(l) - getX(u)) / (getY(l) - getY(u));
  rslope = (getX(r) - getX(u)) / (getY(r) - getY(u));
  for (y = (int)(getY(u) + 0.5); y < (int)(getY(l) + 0.5); y++) {
    x = floor(lpos);
    w = ceil(rpos) - x + 1;
      
    if (target.nchannels == 1)
      DrawRect(target, x, y, w, h, getValue(rgb));
    else
      DrawRect(target, x, y, w, h, getR(rgb), getG(rgb), getB(rgb));
      
    lpos += lslope;
    rpos += rslope;
  }
   
  //Blit lower triangle
  lpos = getX(l);
  rpos = getX(r);
  lslope = (getX(d) - getX(l)) / (getY(d) - getY(l));
  rslope = (getX(d) - getX(r)) / (getY(d) - getY(r));
  for (; y < (int)(getY(d) + 0.5); y++) {
    x = floor(lpos);
    w = ceil(rpos) - x + 1;

    if (target.nchannels == 1)
      DrawRect(target, x, y, w, h, getValue(rgb));
    else
      DrawRect(target, x, y, w, h, getR(rgb), getG(rgb), getB(rgb));
      
    lpos += lslope;
    rpos += rslope;
  }
}
