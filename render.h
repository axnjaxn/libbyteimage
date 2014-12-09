#ifndef _BPJ_BYTEIMAGE_RENDER_H
#define _BPJ_BYTEIMAGE_RENDER_H

#include "byteimage.h"

void DrawRect(ByteImage& target, int x, int y, int w, int h, ByteImage::BYTE v);
void DrawRect(ByteImage& target, int x, int y, int w, int h, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B);
void DrawPoint(ByteImage& target, int x, int y, ByteImage::BYTE v, int sz = 1);
void DrawPoint(ByteImage& target, int x, int y, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz = 1);
void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, ByteImage::BYTE v, int sz = 1);
void DrawLine(ByteImage& target, int ax, int ay, int bx, int by, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B, int sz = 1);

#endif
