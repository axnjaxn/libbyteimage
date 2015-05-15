#include "render.h"
#include <cstring>

using namespace byteimage;

Matrix byteimage::makeColor(double r, double g, double b) {return makePoint(r, g, b);}

inline static double getValue(const Matrix& rgb) {return (rgb.at(0) + rgb.at(1) + rgb.at(2)) / 3.0;}
inline static ByteImage::BYTE getR(const Matrix& rgb) {return ByteImage::clip(rgb.at(0));}
inline static ByteImage::BYTE getG(const Matrix& rgb) {return ByteImage::clip(rgb.at(1));}
inline static ByteImage::BYTE getB(const Matrix& rgb) {return ByteImage::clip(rgb.at(2));}

void byteimage::DrawRect(ByteImage& target, int x, int y, int w, int h,
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

void byteimage::DrawRect(ByteImage& target, int x, int y, int w, int h,
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

void byteimage::DrawRect(ByteImage& target, const Pt2f& ul, const Pt2f& lr, const Color& color) {
  int x = (int)(ul.x + 0.5);
  int y = (int)(ul.y + 0.5);
  int w = (int)(lr.x + 0.5);
  int h = (int)(lr.y + 0.5);

  if (x > w) std::swap(x, w);
  if (y > h) std::swap(y, h);
  w -= x;
  h -= y;

  if (target.nchannels == 1) DrawRect(target, x, y, w, h, color.toGray());
  else DrawRect(target, x, y, w, h, color.r, color.g, color.b);
}

void byteimage::DrawRect(ByteImage& target, const Matrix& ul, const Matrix& lr, const Matrix& rgb) {
  DrawRect(target, ul.toPt2f(), lr.toPt2f(), rgb.toColor());
}

void byteimage::DrawPoint(ByteImage& target, int x, int y,
			  ByteImage::BYTE v, int sz) {
  x = x - (sz >> 1);
  y = y - (sz >> 1);
  int w = sz, h = sz;
  DrawRect(target, x, y, w, h, v);  
}

void byteimage::DrawPoint(ByteImage& target, int x, int y,
			  ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz) {
  x = x - (sz >> 1);
  y = y - (sz >> 1);
  int w = sz, h = sz;
  DrawRect(target, x, y, w, h, R, G, B);
}

void byteimage::DrawPoint(ByteImage& target, const Pt2f& v, const Color& color, int sz) {
  int x = (int)(v.x + 0.5);
  int y = (int)(v.y + 0.5);

  if (target.nchannels == 1) DrawPoint(target, x, y, color.toGray(), sz);
  else DrawPoint(target, x, y, color.r, color.g, color.b, sz);
}

void byteimage::DrawPoint(ByteImage& target, const Matrix& v, const Matrix& rgb, int sz) {
  DrawPoint(target, v.toPt2f(), rgb.toColor(), sz);
}

void byteimage::DrawLine(ByteImage& target, int ax, int ay, int bx, int by,
			 ByteImage::BYTE v, int sz) {
  float slope, tx, ty;

  if (fabs(by - ay) <= fabs(bx - ax)) {
    slope = (float)(by - ay) / (bx - ax);    
    if (ax > bx) {
      std::swap(ax, bx);
      std::swap(ay, by);
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
      std::swap(ax, bx);
      std::swap(ay, by);
    }

    tx = ax; ty = ay;
    while (ty <= by) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), v, sz);
      tx += slope;
      ty++;
    }
  }
}

void byteimage::DrawLine(ByteImage& target, int ax, int ay, int bx, int by,
			 ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz) {
  float slope, tx, ty;

  if (fabs(by - ay) <= fabs(bx - ax)) {
    slope = (float)(by - ay) / (bx - ax);    
    if (ax > bx) {
      std::swap(ax, bx);
      std::swap(ay, by);
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
      std::swap(ax, bx);
      std::swap(ay, by);
    }

    tx = ax; ty = ay;
    while (ty <= by) {
      DrawPoint(target, (int)(tx + 0.5), (int)(ty + 0.5), R, G, B, sz);
      tx += slope;
      ty++;
    }
  }
}

void byteimage::DrawLine(ByteImage& target, const Pt2f& a, const Pt2f& b, const Color& color, int sz) {
  int ax = (int)(a.x + 0.5);
  int ay = (int)(a.y + 0.5);
  int bx = (int)(b.x + 0.5);
  int by = (int)(b.y + 0.5);

  if (target.nchannels == 1) DrawLine(target, ax, ay, bx, by, color.toGray(), sz);
  else DrawLine(target, ax, ay, bx, by, color.r, color.g, color.b, sz);
}

void byteimage::DrawLine(ByteImage& target, const Matrix& a, const Matrix& b, const Matrix& rgb, int sz) {
  DrawLine(target, a.toPt2f(), b.toPt2f(), rgb.toColor());
}

void byteimage::DrawCross(ByteImage& target, int x, int y, 
			  ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b,
			  int radius, int line_size) {
  DrawLine(target, x, y - radius, x, y + radius, r, g, b, line_size);
  DrawLine(target, x - radius, y, x + radius, y, r, g, b, line_size);
}

void byteimage::DrawCross(ByteImage& target, const Pt2f& v, const Color& color,
			  int radius, int line_size) {
  DrawLine(target, Pt2f(v.x, v.y - radius), Pt2f(v.x, v.y + radius), color, line_size);
  DrawLine(target, Pt2f(v.x - radius, v.y), Pt2f(v.x + radius, v.y), color, line_size);
}

void byteimage::DrawCross(ByteImage& target, const Matrix& v, const Matrix& rgb, 
			  int radius, int line_size) {
  DrawCross(target, getX(v), getY(v), getR(rgb), getG(rgb), getB(rgb), radius, line_size);
}

void DrawBezier(ByteImage& target, const std::vector<Pt2f>& pts, const Color& color, int sz, int n) {
  std::vector<Pt2f> interp(pts.size());

  Pt2f v0 = pts[0], v1;
  float t;
  for (int i = 1; i <= n; i++) {
    t = (float)i / n;
    interp.assign(pts.begin(), pts.end());
    for (int order = pts.size() - 1; order >= 1; order--)
      for (int j = 0; j < order; j++)
	interp[j] = (1.0 - t) * interp[j] + t * interp[j + 1];
    v1 = interp[0];
    DrawLine(target, v0, v1, color, sz);
    v0 = v1;
  }
}

void byteimage::DrawBezier(ByteImage& target, const std::vector<Matrix>& pts, const Matrix& rgb, int sz, int n) {
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

void byteimage::DrawCircle(ByteImage& target, const Pt2f& v, const Color& color, float radius) {
  float r2 = radius * radius;
  int r = (int)(v.y + 0.5), c = (int)(v.x + 0.5);
  for (int i = (int)(radius + 1); i >= 0; i--)
    for (int j = (int)(radius + 1); j >= 0; j--)
      if (i * i + j * j <= r2) {
	DrawRect(target, c - j, r - i, 2 * j + 1, 1, color.r, color.g, color.b);
	DrawRect(target, c - j, r + i, 2 * j + 1, 1, color.r, color.g, color.b);
      }
}

void byteimage::DrawCircle(ByteImage& target, const Matrix& v, const Matrix& rgb, double radius) {
  double r2 = radius * radius;
  int r = (int)(getY(v) + 0.5), c = (int)(getX(v) + 0.5);
  for (int i = (int)(radius + 1); i >= 0; i--)
    for (int j = (int)(radius + 1); j >= 0; j--)
      if (i * i + j * j <= r2) {
	DrawRect(target, c - j, r - i, 2 * j + 1, 1, getR(rgb), getG(rgb), getB(rgb));
	DrawRect(target, c - j, r + i, 2 * j + 1, 1, getR(rgb), getG(rgb), getB(rgb));
      }
}

inline static int ifloor(double d) {return (int)d;}
inline static int iceil(double d) {return (int)d + (d != (int)d);}

void byteimage::DrawTriangle(ByteImage& target, const Pt2f& v0, const Pt2f& v1, const Pt2f& v2, const Color& color) {
  Pt2f u, l, r, d;
  u = v0;
  l = v1;
  d = v2;

  if (l.y < u.y) std::swap(u, l);
  if (d.y < u.y) std::swap(u, d);
  if (d.y < l.y) std::swap(l, d);

  //Form two triangles: ulr and lrd
  r = Pt2f(u.x + (l.y - u.y)  * (d.x - u.x) / (d.y - u.y), l.y);
  if (l.x > r.x) std::swap(l, r);

  float lpos, rpos;
  float lslope, rslope;
  int x, y, w, h = 1;

  //Blit upper triangle
  lpos = rpos = u.x;
  lslope = (l.x - u.x) / (l.y - u.y);
  rslope = (r.x - u.x) / (r.y - u.y);
  for (y = (int)(u.y + 0.5); y < (int)(l.y + 0.5); y++) {
    x = ifloor(lpos);
    w = iceil(rpos) - x + 1;
      
    if (target.nchannels == 1) DrawRect(target, x, y, w, h, color.toGray());
    else DrawRect(target, x, y, w, h, color.r, color.g, color.b);
      
    lpos += lslope;
    rpos += rslope;
  }
   
  //Blit lower triangle
  lpos = l.x;
  rpos = r.x;
  lslope = (d.x - l.x) / (d.y - l.y);
  rslope = (d.x - r.x) / (d.y - r.y);
  for (; y < (int)(d.y + 0.5); y++) {
    x = ifloor(lpos);
    w = iceil(rpos) - x + 1;

    if (target.nchannels == 1) DrawRect(target, x, y, w, h, color.toGray());
    else DrawRect(target, x, y, w, h, color.r, color.g, color.b);
      
    lpos += lslope;
    rpos += rslope;
  }
}

void byteimage::DrawTriangle(ByteImage& target, const Matrix& v0, const Matrix& v1, const Matrix& v2, const Matrix& rgb) {
  DrawTriangle(target, v0.toPt2f(), v1.toPt2f(), v2.toPt2f(), rgb.toColor());
}
