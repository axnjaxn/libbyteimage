/*
 * byteimage.h by Brian Jackson
 */

#ifndef _BPJ_BYTEIMAGE_H
#define _BPJ_BYTEIMAGE_H

#include <string>
#ifndef _BYTEIMAGE_NO_MAGICK
#include <Magick++.h>
#endif

namespace byteimage {
  class ByteImage {
  public:
    typedef unsigned char BYTE;
    inline static BYTE clip(double d) {
      return (d > 255)? 255 : (d < 0)? 0 : (BYTE)(d + 0.5);
    }
    inline static BYTE avg(BYTE a, BYTE b) {
      return (a >> 1) + (b >> 1) + ((a & 1) & (b & 1));
    }
    inline static BYTE diff(BYTE a, BYTE b) {
      return (a > b)? a - b : b - a;
    }
    inline static BYTE interp(BYTE a, BYTE b, float t) {
      t = (t < 0)? 0 : (t > 1)? 1 : t;
      return clip((1 - t) * a + t * b);
    }

    BYTE* pixels;
    int nr, nc, nchannels;

    inline BYTE* R() {return pixels;}
    inline BYTE* G() {return pixels + nr * nc;}
    inline BYTE* B() {return pixels + 2 * nr * nc;}
    inline const BYTE* R() const {return pixels;}
    inline const BYTE* G() const {return pixels + nr * nc;}
    inline const BYTE* B() const {return pixels + 2 * nr * nc;}
    static ByteImage combineChannels(const ByteImage& r, const ByteImage& g, const ByteImage& b);

  protected:
    void setLightness(int r, int c, BYTE l); //Assumes RGB, 3 channel encoding
  
  public:
    ByteImage();
#ifndef _BYTEIMAGE_NO_MAGICK
    ByteImage(std::string fn);
#endif
    ByteImage(int nr, int nc, int nchannels = 1, BYTE val = 0);
    ByteImage(const BYTE* pixels, int nr, int nc, int nchannels = 1);
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
    inline BYTE& operator[](int i) {return pixels[i];}
    inline BYTE operator[](int i) const {return pixels[i];}
    inline BYTE& at(int r, int c, int ch = 0) {return pixels[nr * nc * ch + r * nc + c];}
    inline BYTE at(int r, int c, int ch = 0) const {return pixels[nr * nc * ch + r * nc + c];};
    BYTE atBounded(int r, int c, int ch = 0) const;

    void fill(BYTE b);

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

    BYTE bilin(double r, double c, int ch = 0) const;
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
  
    ByteImage getLightness() const;
    ByteImage setLightness(const ByteImage& I) const;
  };

  void rgb2hsl(ByteImage::BYTE r, ByteImage::BYTE g, ByteImage::BYTE b,
	       float &h, float &s, float &l);

  void hsl2rgb(float h, float s, float l,
	       ByteImage::BYTE &r, ByteImage::BYTE &g, ByteImage::BYTE &b);
}
		  

#endif
