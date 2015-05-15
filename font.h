#ifndef _BPJ_BYTEIMAGE_FONT_H
#define _BPJ_BYTEIMAGE_FONT_H

#include "byteimage.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace byteimage {
  class TextRenderer {
  protected:
    static FT_Library library;
    FT_Face face;

    void drawGlyph(int pen_y, int pen_x, ByteImage& target, int r, int c, Byte rgb[3]) const;

  public:
    static int dpi;//72 by default

    TextRenderer(const char* name, int size = 12, int index = 0);
    virtual ~TextRenderer();

    void drawUnkerned(ByteImage& target, const char* str, int r, int c, Byte v = 255) const;
    void drawUnkerned(ByteImage& target, const char* str, int r, int c, Byte R, Byte G, Byte B) const;

    void draw(ByteImage& target, const char* str, int r, int c, Byte v = 255) const;
    void draw(ByteImage& target, const char* str, int r, int c, Byte R, Byte G, Byte B) const;
  
    void drawCentered(ByteImage& target, const char* str, int r, int c, Byte v = 255) const;
    void drawCentered(ByteImage& target, const char* str, int r, int c, Byte R, Byte G, Byte B) const;

    void getBox(const char* str, int& x, int& y, int& w, int& h) const;
  };
}

#endif
