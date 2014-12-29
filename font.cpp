#include "font.h"
#include <cstdio>

int TextRenderer::dpi = 72;
FT_Library TextRenderer::library = NULL;

TextRenderer::TextRenderer(const char* name, int size, int index) {
  int error;

  if (!library) {
    if (error = FT_Init_FreeType(&library)) {
      printf("FreeType could not be initialized: code %d\n", error);
      return;
    }
  }
  
  error = FT_New_Face(library, name, index, &face);
  if (error) {
    printf("Could not load font %s: code %d\n", name, error);
    return;
  }

  error = FT_Set_Char_Size(face,
			   0, size*64, //width and height in 1/64 pt
			   dpi, dpi);  //Horiz. and vertical resolution
  if (error) {
    printf("Could not set char size: code %d\n", error);
    return;
  }
}
  

TextRenderer::~TextRenderer() {
  FT_Done_Face(face);
}

void TextRenderer::draw(ByteImage& target, const char* str, int r, int c, ByteImage::BYTE v) const {
  draw(target, str, r, c, v, v, v);
}

void TextRenderer::draw(ByteImage& target, const char* str, int r, int c, 
			ByteImage::BYTE R, ByteImage::BYTE G, ByteImage::BYTE B) const {
  if (!library || !face) return;

  FT_GlyphSlot slot = face->glyph;
  int pen_x = c, pen_y = r;
  
  ByteImage::BYTE rgb[3];
  rgb[0] = R;
  rgb[1] = G;
  rgb[2] = B;

  for (; *str; str++) {
    if (FT_Load_Char(face, *str, FT_LOAD_RENDER)) continue;
    else if (slot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) continue;

    //Draw the bitmap
    unsigned char* px = (unsigned char*)slot->bitmap.buffer;
    float v;
    for (int ch = 0; ch < target.nchannels; ch++)
      for (int r1 = 0, x, y; r1 < slot->bitmap.rows; r1++)
	for (int c1 = 0; c1 < slot->bitmap.width; c1++) {
	  x = pen_x + slot->bitmap_left + c1;
	  y = pen_y - slot->bitmap_top + r1;
	  if (!target.inBounds(y, x)) continue;
	  
	  v = px[r1 * slot->bitmap.pitch + c1] / 255.0;
	  target.at(y, x, ch) = ByteImage::clip(rgb[ch] * v + target.at(y, x, ch) * (1 - v));
	}

    pen_x += slot->advance.x >> 6;
  }
}
