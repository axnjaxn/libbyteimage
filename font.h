#ifndef _BPJ_BYTEIMAGE_FONT_H
#define _BPJ_BYTEIMAGE_FONT_H

#include "byteimage.h"
#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer {
protected:
  static FT_Library library;
  FT_Face face;

public:
  static int dpi;//72 by default

  TextRenderer(const char* name, int size = 12, int index = 0);
  virtual ~TextRenderer();

  void draw(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE v = 255) const;
  void draw(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B) const;
};

#endif
