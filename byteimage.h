/*
 * byteimage.h by Brian Jackson
 */

#ifndef _BPJ_BYTEIMAGE_H
#define _BPJ_BYTEIMAGE_H

#include "types.h"
#include <string>
#ifndef _BYTEIMAGE_NO_MAGICK
#include <Magick++.h>
#endif

namespace byteimage {
  class ByteImage {
  public:
    Byte* pixels;
    int nr, nc, nchannels;

    inline Byte* R() {return pixels;}
    inline Byte* G() {return pixels + nr * nc;}
    inline Byte* B() {return pixels + 2 * nr * nc;}
    inline const Byte* R() const {return pixels;}
    inline const Byte* G() const {return pixels + nr * nc;}
    inline const Byte* B() const {return pixels + 2 * nr * nc;}
    static ByteImage combineChannels(const ByteImage& r, const ByteImage& g, const ByteImage& b);

  protected:
    void setLightness(int r, int c, Byte l); //Assumes RGB, 3 channel encoding
  
  public:
    ByteImage();
#ifndef _BYTEIMAGE_NO_MAGICK
    ByteImage(std::string fn);
#endif
    ByteImage(int nr, int nc, int nchannels = 1, Byte val = 0);
    ByteImage(const Byte* pixels, int nr, int nc, int nchannels = 1);
#ifndef _BYTEIMAGE_NO_MAGICK
    ByteImage(Magick::Image& img);
#endif
    ByteImage(const ByteImage& img);
    ByteImage(ByteImage&& img);
    ByteImage(FILE* fp, int nr, int nc, int nchannels);
    virtual ~ByteImage();

    ByteImage& operator=(const ByteImage& img);
    ByteImage& operator=(ByteImage&& img);

    inline int inBounds(int r, int c) const {return (r >= 0 && r < nr && c >= 0 && c <= nc);}
    inline int size() const {return nr * nc * nchannels;}
    inline Byte& operator[](int i) {return pixels[i];}
    inline Byte operator[](int i) const {return pixels[i];}
    inline Byte& at(int r, int c, int ch = 0) {return pixels[nr * nc * ch + r * nc + c];}
    inline Byte at(int r, int c, int ch = 0) const {return pixels[nr * nc * ch + r * nc + c];};
    Byte atBounded(int r, int c, int ch = 0) const;

    void fill(Byte b);

    void resize(int nr, int nc);
    ByteImage resized(int nr, int nc) const;

    static ByteImage avg(const ByteImage& img1, const ByteImage& img2);
    static ByteImage diff(const ByteImage& img1, const ByteImage& img2);
    static ByteImage interp(const ByteImage& img1, const ByteImage& img2, float t);

    ByteImage toLightness() const;
    ByteImage toGrayscale() const;
    ByteImage toColor() const;
    ByteImage selectChannel(int ch) const;
    ByteImage subImage(int r, int c, int nr, int nc) const;

    ByteImage u2() const;
    ByteImage d2() const;

    Byte bilin(double r, double c, int ch = 0) const;
    ByteImage sampled(double factor) const;
    ByteImage scaled(int nr, int nc) const;
    ByteImage aspectScaled(int nr, int nc) const;//Guarantees returned image has bounds less than or equal to given dimensions

    ByteImage rotatedCW() const;
    ByteImage rotatedCCW() const;
  
#ifndef _BYTEIMAGE_NO_MAGICK
    void load_filename(std::string fn);
    void save_filename(std::string fn) const;
#endif
    void writeHeader(FILE* fp) const;
    void write(FILE* fp) const;

    void blit(const ByteImage& src, int destr, int destc);
    void blend(const ByteImage& color, const ByteImage& alpha, int destr, int destc);
    void blitSampled(const ByteImage& src, float sx, float sy, int dr, int dc);
  
    ByteImage getLightness() const;
    ByteImage setLightness(const ByteImage& I) const;
  };

  void rgb2hsl(Byte r, Byte g, Byte b, float &h, float &s, float &l);
  void hsl2rgb(float h, float s, float l, Byte &r, Byte &g, Byte &b);
}
		  

#endif
