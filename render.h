#ifndef _BPJ_BYTEIMAGE_RENDER_H
#define _BPJ_BYTEIMAGE_RENDER_H

#include "byteimage.h"
#include "matrix.h"
#include <vector>

namespace byteimage {
  void DrawRect(ByteImage& target, int x, int y, int w, int h, Byte v);
  void DrawRect(ByteImage& target, int x, int y, int w, int h, Byte R, Byte G, Byte B);
  void DrawRect(ByteImage& target, const Pt2f& ul, const Pt2f& lr, const Color& color);

  void DrawPoint(ByteImage& target, int x, int y, Byte v, int sz = 1);
  void DrawPoint(ByteImage& target, int x, int y, Byte R, Byte G, Byte B, int sz = 1);
  void DrawPoint(ByteImage& target, const Pt2f& v, const Color& color, int sz = 1);

  void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, Byte v, int sz = 1);
  void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, Byte R, Byte G, Byte B, int sz = 1);
  void DrawLine(ByteImage& target, const Pt2f& a, const Pt2f& b, const Color& color, int sz = 1);

  void DrawCross(ByteImage& target, int x, int y, Byte r, Byte g, Byte b, int radius = 2, int line_size = 1);
  void DrawCross(ByteImage& target, const Pt2f& v, const Color& color, int radius = 2, int line_size = 1);

  void DrawBezier(ByteImage& target, const std::vector<Pt2f>& pts, const Color& color, int sz = 1, int n = 16);
  void DrawCircle(ByteImage& target, const Pt2f& v, const Color& color, float radius);
  void DrawTriangle(ByteImage& target, const Pt2f& v0, const Pt2f& v1, const Pt2f& v2, const Color& color);
}

#endif
