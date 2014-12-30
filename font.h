#ifndef _BPJ_BYTEIMAGE_FONT_H
#define _BPJ_BYTEIMAGE_FONT_H

#include "byteimage.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer {
protected:
  static FT_Library library;
  FT_Face face;

  void drawGlyph(int pen_y, int pen_x, ByteImage& target, int r, int c, ByteImage::BYTE rgb[3]) const;

public:
  static int dpi;//72 by default

  TextRenderer(const char* name, int size = 12, int index = 0);
  virtual ~TextRenderer();

  void drawUnkerned(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE v = 255) const;
  void drawUnkerned(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B) const;

  void draw(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE v = 255) const;
  void draw(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B) const;
  
  void getBox(const char* str, int& x, int& y, int& w, int& h) const;
};

#endif
