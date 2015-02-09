#ifndef _BPJ_BYTEIMAGE_RENDER_H
#define _BPJ_BYTEIMAGE_RENDER_H

#include "byteimage.h"
#include "matrix.h"
#include <vector>

Matrix makeColor(double r, double g, double b);//On a [0 - 255] scale

void DrawRect(ByteImage& target, int x, int y, int w, int h, ByteImage::BYTE v);
void DrawRect(ByteImage& target, int x, int y, int w, int h, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B);
void DrawRect(ByteImage& target, const Matrix& ul, const Matrix& lr, const Matrix& rgb);

void DrawPoint(ByteImage& target, int x, int y, ByteImage::BYTE v, int sz = 1);
void DrawPoint(ByteImage& target, int x, int y, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz = 1);
void DrawPoint(ByteImage& target, const Matrix& v, const Matrix& rgb, int sz = 1);

void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, ByteImage::BYTE v, int sz = 1);
void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz = 1);
void DrawLine(ByteImage& target, const Matrix& a, const Matrix& b, const Matrix& rgb, int sz = 1);

void DrawCross(ByteImage& target, int x, int y, ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b, int radius = 2, int line_size = 1);
void DrawCross(ByteImage& target, const Matrix& v, const Matrix& rgb, int radius = 2, int line_size = 1);

void DrawBezier(ByteImage& target, const std::vector<Matrix>& pts, const Matrix& rgb, int sz = 1, int n = 16);

void DrawCircle(ByteImage& target, const Matrix& v, const Matrix& rgb, double radius);
void DrawTriangle(ByteImage& target, const Matrix& v0, const Matrix& v1, const Matrix& v2, const Matrix& rgb);

#endif
