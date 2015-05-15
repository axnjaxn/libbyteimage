/*
 * byteimage.cpp by Brian Jackson
 */

#include "byteimage.h"
#include <cstdio>
#include <cstring>
#ifndef _BYTEIMAGE_NO_MAGICK
using namespace Magick;
#include <Magick++.h>
#endif

using namespace byteimage;

template <typename tn>
inline static tn min(tn a, tn b) {return (a <= b)? a : b;}

ByteImage::ByteImage() : nr(0), nc(0), nchannels(0), pixels(nullptr) { }

#ifndef _BYTEIMAGE_NO_MAGICK
ByteImage::ByteImage(std::string fn) {
  pixels = nullptr;
  load_filename(fn);
}
#endif

ByteImage::ByteImage(int nr, int nc, int nchannels, Byte val) : nr(nr), nc(nc), nchannels(nchannels) {
  pixels = new Byte [size()];
  memset(pixels, val, size());
}

ByteImage::ByteImage(const Byte* pixels, int nr, int nc, int nchannels) : nr(nr), nc(nc), nchannels(nchannels) {
  this->pixels = new Byte [size()];
  memcpy(this->pixels, pixels, size());
}

#ifndef _BYTEIMAGE_NO_MAGICK
ByteImage::ByteImage(Magick::Image& img) : nr(img.rows()), nc(img.columns()), nchannels(3) {
  pixels = new Byte [size()];

  Magick::Pixels cache(img);
  Magick::PixelPacket* packet = cache.get(0, 0, nc, nr);

  //nshifts will allow a nice transition between quantizations
  int nshifts = 0;
  for (int i = MaxRGB >> 8; i & 1; i = i >> 1) nshifts++;
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      at(r, c, 0) = (Byte)(packet[r * nc + c].red >> nshifts);
      at(r, c, 1) = (Byte)(packet[r * nc + c].green >> nshifts);
      at(r, c, 2) = (Byte)(packet[r * nc + c].blue >> nshifts);
    }	

  cache.sync();
}
#endif

ByteImage::ByteImage(FILE* fp, int nr, int nc, int nchannels) : nr(nr), nc(nc), nchannels(nchannels) {
  pixels = new Byte [nr * nc * nchannels];
  fread(pixels, 1, nr * nc * nchannels, fp);
}

ByteImage::ByteImage(const ByteImage& img) : nr(0), nc(0), nchannels(0), pixels(nullptr) {*this = img;}

ByteImage::ByteImage(ByteImage&& img) : nr(0), nc(0), nchannels(0), pixels(nullptr) {*this = img;}

ByteImage::~ByteImage() {
  delete [] pixels;
}

ByteImage& ByteImage::operator=(const ByteImage& img) {
  if (size() != img.size()) {
    delete [] pixels;
    pixels = new Byte [img.size()];
  }

  nr = img.nr;
  nc = img.nc;
  nchannels = img.nchannels;
  memcpy(pixels, img.pixels, size());
  
  return *this;
}

ByteImage& ByteImage::operator=(ByteImage&& img) {
  delete [] pixels;
  nr = img.nr; img.nr = 0;
  nc = img.nc; img.nc = 0;
  nchannels = img.nchannels; img.nchannels = 0;
  pixels = img.pixels; img.pixels = nullptr;
  
  return *this;
}

Byte ByteImage::atBounded(int r, int c, int ch) const {
  if (r < 0) r = 0;
  else if (r >= nr) r = nr - 1;
  
  if (c < 0) c = 0;
  else if (c >= nc) c = nc - 1;
  
  return at(r, c, ch);
}

void ByteImage::fill(Byte b) {
  memset(pixels, b, nr * nc * nchannels);
}

void ByteImage::resize(int nr, int nc) {
  int mr = (nr < this->nr)? nr : this->nr;
  int mc = (nc < this->nc)? nc : this->nc;

  Byte* pixels = new Byte [nr * nc * nchannels];

  memset(pixels, 0, nr * nc * nchannels);
  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < mr; r++)
      memcpy(pixels + ch * nr * nc + r * nc,
	     this->pixels + ch * this->nr * this->nc + r * this->nc,
	     mc);
  
  delete [] this->pixels;
  this->pixels = pixels;
  this->nr = nr;
  this->nc = nc;
}

ByteImage ByteImage::resized(int nr, int nc) const {
  ByteImage result(*this);
  result.resize(nr, nc);
  return result;
}

ByteImage ByteImage::avg(const ByteImage& img1, const ByteImage& img2) {
  if (img1.nchannels != img2.nchannels) return avg(img1.toColor(), img2.toColor());

  ByteImage result(min(img1.nr, img2.nr),
		   min(img1.nc, img2.nc),
		   img1.nchannels);

  for (int ch = 0; ch < result.nchannels; ch++)
    for (int r = 0; r < result.nr; r++)
      for (int c = 0; c < result.nc; c++)
	result.at(r, c, ch) = byteimage::avg(img1.at(r, c, ch), img2.at(r, c, ch));

  return result;
}

ByteImage ByteImage::diff(const ByteImage& img1, const ByteImage& img2) {
  if (img1.nchannels != img2.nchannels) return diff(img1.toColor(), img2.toColor());

  ByteImage result(min(img1.nr, img2.nr),
		   min(img1.nc, img2.nc),
		   img1.nchannels);

  for (int ch = 0; ch < result.nchannels; ch++)
    for (int r = 0; r < result.nr; r++)
      for (int c = 0; c < result.nc; c++)
	result.at(r, c, ch) = byteimage::diff(img1.at(r, c, ch), img2.at(r, c, ch));

  return result;
}

ByteImage ByteImage::interp(const ByteImage& img1, const ByteImage& img2, float t) {
  if (img1.nchannels != img2.nchannels) return interp(img1.toColor(), img2.toColor(), t);

  ByteImage result(min(img1.nr, img2.nr),
		   min(img1.nc, img2.nc),
		   img1.nchannels);

  for (int ch = 0; ch < result.nchannels; ch++)
    for (int r = 0; r < result.nr; r++)
      for (int c = 0; c < result.nc; c++)
	result.at(r, c, ch) = byteimage::interp(img1.at(r, c, ch), img2.at(r, c, ch), t);

  return result;
}

ByteImage ByteImage::toLightness() const {
  if (nchannels == 1) return *this;

  ByteImage img(nr, nc);

  int max, min;
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      max = 0; min = 255;
      for (int ch = 0; ch < nchannels; ch++) {
	if (at(r, c, ch) > max) max = at(r, c, ch);
	if (at(r, c, ch) < min) min = at(r, c, ch);
      }
      img.at(r, c) = (Byte)((min + max) >> 1);
    }

  return img;
}

ByteImage ByteImage::toGrayscale() const {
  if (nchannels == 1) return *this;

  const Byte *r = R(), *g = G(), *b = B();
  ByteImage img(nr, nc);

  for (int i = 0; i < nr * nc; i++)
    img.pixels[i] = (Byte)((r[i] + g[i] + b[i]) / 3);

  return img;
}

ByteImage ByteImage::toColor() const {
  if (nchannels == 3) return *this;
  
  ByteImage img(nr, nc, 3);
  memcpy(img.pixels, pixels, nr * nc);
  memcpy(img.pixels + nr * nc, pixels, nr * nc);
  memcpy(img.pixels + 2 * nr * nc, pixels, nr * nc);

  return img;
}

ByteImage ByteImage::selectChannel(int ch) const {
  return ByteImage(pixels + nr * nc * ch, nr, nc, 1);
}

ByteImage ByteImage::subImage(int y, int x, int rows, int cols) const {
  ByteImage sub(rows, cols, nchannels);

  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < rows; r++)
      memcpy(sub.pixels + rows * cols * ch + r * cols, 
	     pixels + nr * nc * ch + (y + r) * nc + x, 
	     cols);

  return sub;
}

ByteImage ByteImage::u2() const {
  ByteImage img(nr << 1, nc << 1, nchannels);

  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < img.nr; r += 2) {
      for (int c = 0; c < img.nc; c += 2)
	img.at(r, c, ch) = img.at(r, c + 1, ch) = at(r >> 1, c >> 1, ch);
      memcpy(img.pixels + ch * img.nr * img.nc + (r + 1) * img.nc,
	     img.pixels + ch * img.nr * img.nc + r * img.nc,
	     img.nc);
    }

  return img;
}

ByteImage ByteImage::d2() const {
  ByteImage img(nr >> 1, nc >> 1, nchannels);

  int val;
  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < img.nr; r++)
      for (int c = 0; c < img.nc; c++) {
	val = at(r << 1, c << 1, ch) 
	  + at(r << 1 | 1, c << 1, ch)
	  + at(r << 1 | 1, c << 1 | 1, ch)
	  + at(r << 1, c << 1 | 1, ch);
	img.at(r, c, ch) = (Byte)(val >> 2);
      }	

  return img;
}

Byte ByteImage::bilin(double r, double c, int ch) const {
  const double eps = 0.0001;

#ifdef BYTEIMAGE_SMEAR_EDGES
  //Boundary check
  if (r < 0) r = 0;
  else if (r >= nr - 1) r = nr - 1 - eps;
  if (c < 0) c = 0;
  else if (c >= nc - 1) c = nc - 1 - eps;
#else
  //Boundary check
  if (r < 0 || r >= nr - 1 || c < 0 || c >= nc - 1) return 0;
#endif
  
  //Split into integer and fractional components
  int R = (int)r; r -= R;
  int C = (int)c; c -= C;

  double sum = 0.0;
  sum += at(R, C, ch) * (1 - r) * (1 - c);
  sum += at(R + 1, C, ch) * r * (1 - c);
  sum += at(R, C + 1, ch) * (1 - r) * c;
  sum += at(R + 1, C + 1, ch) * r * c;
  if (sum > 255) sum = 255;

  return (Byte)(sum + 0.5);
}

ByteImage ByteImage::sampled(double factor) const {
  ByteImage dest((int)(nr * factor + 0.5), (int)(nc * factor + 0.5), nchannels);
  
  factor = 1.0 / factor;
  for (int ch = 0; ch < dest.nchannels; ch++)
    for (int r = 0; r < dest.nr; r++)
      for (int c = 0; c < dest.nc; c++)
	dest.at(r, c, ch) = bilin(factor * r, factor * c, ch);

  return dest;
}

ByteImage ByteImage::scaled(int nr, int nc) const {
  ByteImage scaled(nr, nc, this->nchannels);
  double xfact = (double)this->nc / nc;
  double yfact = (double)this->nr / nr;

  double* colors = new double [scaled.nchannels];
  double x0, y0, x1, y1, w, h;
  for (int r = 0; r < nr; r++) {
    for (int c = 0; c < nc; c++) {
      memset(colors, 0, sizeof(double) * scaled.nchannels);

      y0 = r * yfact;
      y1 = y0 + yfact;
      for (int sr = (int)y0; sr < y1 && sr < this->nr; y0 = ++sr) {
	if (sr == (int)y1)
	  h = y1 - y0;
	else
	  h = 1 - (y0 - sr);
	
	x0 = c * xfact;
	x1 = x0 + xfact;
	for (int sc = (int)x0; sc < x1 && sc < this->nc; x0 = ++sc) {
	  if (sc == (int)x1)
	    w = x1 - x0;
	  else
	    w = 1 - (x0 - sc);

	  for (int ch = 0; ch < scaled.nchannels; ch++)
	    colors[ch] += at(sr, sc, ch) * w * h;
	}
      }
      
      for (int ch = 0; ch < scaled.nchannels; ch++)
	scaled.at(r, c, ch) = clip(colors[ch] / (xfact * yfact));
    }
  }
  delete [] colors;

  return scaled;
}

ByteImage ByteImage::aspectScaled(int nr, int nc) const {
  double ysc = (double)nr / this->nr;
  double xsc = (double)nc / this->nc;
  return (xsc <= ysc)? scaled(this->nr * xsc, nc) : scaled(nr, this->nc * ysc);
}

ByteImage ByteImage::rotatedCW() const {
  ByteImage result(nc, nr, nchannels);

  for (int ch = 0; ch < result.nchannels; ch++)
    for (int r = 0; r < result.nr; r++)
      for (int c = 0; c < result.nc; c++)
	result.at(r, c, ch) = at(nr - c - 1, r, ch);

  return result;
}

ByteImage ByteImage::rotatedCCW() const {
  ByteImage result(nc, nr, nchannels);

  for (int ch = 0; ch < result.nchannels; ch++)
    for (int r = 0; r < result.nr; r++)
      for (int c = 0; c < result.nc; c++)
	result.at(r, c, ch) = at(c, nc - r - 1, ch);

  return result;
}

#ifndef _BYTEIMAGE_NO_MAGICK
void ByteImage::load_filename(std::string fn) {
  FILE* fp = fopen(fn.c_str(), "rb");
  if (!fp) {
    printf("Could not load %s.\n", fn.c_str());
    exit(1);
  }
  fclose(fp);

  Magick::Image img(fn);

  nr = img.rows();
  nc = img.columns();
  nchannels = 3;
  pixels = new Byte [size()];

  Magick::Pixels cache(img);
  Magick::PixelPacket* packet = cache.get(0, 0, nc, nr);

  //nshifts will allow a nice transition between quantizations
  int nshifts = 0;
  for (int i = MaxRGB >> 8; i & 1; i = i >> 1) nshifts++;
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      at(r, c, 0) = (Byte)(packet[r * nc + c].red >> nshifts);
      at(r, c, 1) = (Byte)(packet[r * nc + c].green >> nshifts);
      at(r, c, 2) = (Byte)(packet[r * nc + c].blue >> nshifts);
    }	

  cache.sync();
}

void ByteImage::save_filename(std::string fn) const {
  if (nchannels != 3) {
    toColor().save_filename(fn);
    return;
  }

  Magick::Image img(Magick::Geometry(nc, nr), Magick::Color(0, 0, 0, 0));

  Magick::Pixels cache(img);
  Magick::PixelPacket* packet = cache.get(0, 0, nc, nr);

  int nshifts = 0;
  for (int i = MaxRGB >> 8; i & 1; i = i >> 1) nshifts++;
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      packet[r * nc + c].red = at(r, c, 0) << nshifts;
      packet[r * nc + c].green = at(r, c, 1) << nshifts;
      packet[r * nc + c].blue = at(r, c, 2) << nshifts;
    }	
  
  cache.sync();

  img.write(fn);
}
#endif

void ByteImage::writeHeader(FILE* fp) const {
  fwrite(&nr, 1, sizeof(int), fp);
  fwrite(&nc, 1, sizeof(int), fp);
  fwrite(&nchannels, 1, sizeof(int), fp);
}

void ByteImage::write(FILE* fp) const {
  fwrite(pixels, 1, nr * nc * nchannels, fp);
}

void ByteImage::blit(const ByteImage& src, int destr, int destc) {
  if (src.nchannels != nchannels) {
    if (nchannels == 1) blit(src.toGrayscale(), destr, destc);
    else if (nchannels == 3) blit(src.toColor(), destr, destc);
    return;
  }

  int x = destc, y = destr, w = src.nc, h = src.nr;

  if (x < 0) {
    w += x;
    x = 0;
  }
  if (x + w > nc)
    w = nc - x;
  if (w <= 0) return;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > nr)
    h = nr - y;  
  if (h <= 0) return;

  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < h; r++)
      memcpy(pixels + ch * nr * nc + (r + y) * nc + x, 
	     src.pixels + ch * src.nr * src.nc + (r + y - destr) * src.nc + x - destc,
	     w);
}

void ByteImage::blend(const ByteImage& color, const ByteImage& alpha, int destr, int destc) {
  if (color.nchannels != nchannels) {
    if (nchannels == 1) blend(color.toGrayscale(), alpha, destr, destc);
    else if (nchannels == 3) blend(color.toColor(), alpha, destr, destc);
    return;
  }

  int x = destc, y = destr, w = color.nc, h = color.nr;

  if (x < 0) {
    w += x;
    x = 0;
  }
  if (x + w > nc)
    w = nc - x;
  if (w <= 0) return;

  if (y < 0) {
    h += y;
    y = 0;
  }
  if (y + h > nr)
    h = nr - y;  
  if (h <= 0) return;

  for (int ch = 0; ch < nchannels; ch++)
    for (int r = 0; r < h; r++)
      for (int c = 0; c < w; c++)
	at(r + y, c + x, ch) = byteimage::interp(at(r + y, c + x, ch), 
						 color.at(r + y - destr, c + x - destc, ch), 
						 alpha.at(r + y - destr, c + x - destc) / 255.0);
}

ByteImage ByteImage::combineChannels(const ByteImage& r, const ByteImage& g, const ByteImage& b) {
  ByteImage result(r.nr, r.nc, 3);
  memcpy(result.R(), r.pixels, result.nr * result.nc);
  memcpy(result.G(), g.pixels, result.nr * result.nc);
  memcpy(result.B(), b.pixels, result.nr * result.nc);
  return result;
}

void ByteImage::setLightness(int r, int c, Byte l) {
  Byte R, G, B;
  float h, s, l0;
  
  R = at(r, c, 0);
  G = at(r, c, 1);
  B = at(r, c, 2);

  rgb2hsl(R, G, B, h, s, l0);
  hsl2rgb(h, s, l / 255.0, R, G, B);

  at(r, c, 0) = R;
  at(r, c, 1) = G;
  at(r, c, 2) = B;
}

ByteImage ByteImage::getLightness() const {
  if (nchannels != 3) return *this;

  ByteImage result(nr, nc);
  
  const Byte *R = this->R(), *G = this->G(), *B = this->B();
  for (int i = 0; i < nr * nc; i++) {
    //Get max and min of the rgb values
    Byte max = R[i];
    if (G[i] > max) max = G[i]; 
    if (B[i] > max) max = B[i];
    
    Byte min = R[i];
    if (G[i] < min) min = G[i]; 
    if (B[i] < min) min = B[i];
    
    //Find lightness
    result[i] = byteimage::avg(max, min);
  }

  return result;
}

ByteImage ByteImage::setLightness(const ByteImage& L) const {
  if (nchannels == 1) return *this;
  
  ByteImage result(*this);
  int nr = (this->nr < L.nr)? this->nr : L.nr;
  int nc = (this->nc < L.nc)? this->nc : L.nc;
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++)
      result.setLightness(r, c, L.at(r, c));
  return result;
}

void byteimage::rgb2hsl(Byte r, Byte g, Byte b, float &h, float &s, float &l) {
  Byte min, max;
  float c;

  max = (r > g)? r : g;
  max = (max > b)? max : b;

  min = (r < g)? r : g;
  min = (min < b)? min : b;
  
  c = max - min;

  //Find hue in degrees
  if (c == 0) h = 0;
  else if (max == r) {
    h = (g - b) / c;
    if (b > g) h += 6.0;
  }
  else if (max == g) h = (b - r) / c + 2;
  else h = (r - g) / c + 4;

  h = 60 * h;
  
  //Find lightness
  const float d255 = 3.921568627e-3;
  const float d510 = 0.5 * d255;
  l = (max + min) * d510;
  
  //Find saturation
  if (l > 0.0 && l < 1.0) {
    s = c * d255;
    if (l > 0.5) s /= 2 - 2 * l;
    else s /= 2 * l;
  }
  else
    s = 0.0;
}

void byteimage::hsl2rgb(float h, float s, float l, Byte &R, Byte &G, Byte &B) {
  float c = (l <= 0.5)? c = 2 * l * s : (2 - 2 * l) * s;

  const float d60 = 1.0 / 60.0;
  h = h * d60;
  int htype = (int)h;
  h  = (htype & 1) + (h - htype);//h = fmod(h, 2)
  h = (h > 1)? h - 1 : 1 - h;
  float x = c * (1 - h);

  float r, g, b;
  switch (htype) {
  default: r = 0; g = 0; b = 0; break;
  case 0: r = c; g = x; b = 0; break;
  case 1: r = x; g = c; b = 0; break;
  case 2: r = 0; g = c; b = x; break;
  case 3: r = 0; g = x; b = c; break;
  case 4: r = x; g = 0; b = c; break;
  case 5: r = c; g = 0; b = x; break;
  }    

  x = l - 0.5 * c;
  R = clip(255.0f * (r + x));
  G = clip(255.0f * (g + x));
  B = clip(255.0f * (b + x));
}
