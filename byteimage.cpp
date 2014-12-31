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

template <typename tn>
inline tn min(tn a, tn b) {return (a <= b)? a : b;}

ByteImage::ByteImage() {
  pixels = NULL; 
  nr = nc = nchannels = 0;
}

#ifndef _BYTEIMAGE_NO_MAGICK
ByteImage::ByteImage(std::string fn) {
  pixels = NULL;
  load_filename(fn);
}
#endif

ByteImage::ByteImage(int nr, int nc, int nchannels, BYTE val) {
  this->nr = nr;
  this->nc = nc;
  this->nchannels = nchannels;
  pixels = new BYTE [size()];
  memset(pixels, val, size());
}

ByteImage::ByteImage(const BYTE* pixels, int nr, int nc, int nchannels) {
  this->nr = nr;
  this->nc = nc;
  this->nchannels = nchannels;
  this->pixels = new BYTE [size()];
  memcpy(this->pixels, pixels, size());
}

#ifndef _BYTEIMAGE_NO_MAGICK
ByteImage::ByteImage(Magick::Image& img) {
  nr = img.rows();
  nc = img.columns();
  nchannels = 3;
  pixels = new BYTE [size()];

  Magick::Pixels cache(img);
  Magick::PixelPacket* packet = cache.get(0, 0, nc, nr);

  //nshifts will allow a nice transition between quantizations
  int nshifts = 0;
  for (int i = MaxRGB >> 8; i & 1; i = i >> 1) nshifts++;
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      at(r, c, 0) = (BYTE)(packet[r * nc + c].red >> nshifts);
      at(r, c, 1) = (BYTE)(packet[r * nc + c].green >> nshifts);
      at(r, c, 2) = (BYTE)(packet[r * nc + c].blue >> nshifts);
    }	

  cache.sync();
}
#endif

ByteImage::ByteImage(FILE* fp, int nr, int nc, int nchannels) {
  this->nr = nr;
  this->nc = nc;
  this->nchannels = nchannels;
  pixels = new BYTE [nr * nc * nchannels];
  fread(pixels, 1, nr * nc * nchannels, fp);
}

ByteImage::ByteImage(const ByteImage& img) {
  pixels = NULL;
  *this = img;
}

ByteImage::~ByteImage() {
  if (pixels) delete [] pixels;
}

ByteImage& ByteImage::operator=(const ByteImage& img) {
  if (pixels) delete [] pixels;

  nr = img.nr;
  nc = img.nc;
  nchannels = img.nchannels;
  pixels = new BYTE [size()];
  memcpy(pixels, img.pixels, size());
  
  return *this;
}

ByteImage::BYTE ByteImage::atBounded(int r, int c, int ch) const {
  if (r < 0) r = 0;
  else if (r >= nr) r = nr - 1;
  
  if (c < 0) c = 0;
  else if (c >= nc) c = nc - 1;
  
  return at(r, c, ch);
}

void ByteImage::resize(int nr, int nc) {
  int mr = (nr < this->nr)? nr : this->nr;
  int mc = (nc < this->nc)? nc : this->nc;

  BYTE* pixels = new BYTE [nr * nc * nchannels];

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
	result.at(r, c, ch) = avg(img1.at(r, c, ch), img2.at(r, c, ch));

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
	result.at(r, c, ch) = diff(img1.at(r, c, ch), img2.at(r, c, ch));

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
	result.at(r, c, ch) = interp(img1.at(r, c, ch), img2.at(r, c, ch), t);

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
      img.at(r, c) = (BYTE)((min + max) >> 1);
    }

  return img;
}

ByteImage ByteImage::toValue() const {
  if (nchannels == 1) return *this;

  ByteImage img(nr, nc);

  BYTE max;
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      max = 0;
      for (int ch = 0; ch < nchannels; ch++)
	if (at(r, c, ch) > max) max = at(r, c, ch);
      img.at(r, c) = max;
    }

  return img;
}

ByteImage ByteImage::toGrayscale() const {
  if (nchannels == 1) return *this;

  const BYTE *r = R(), *g = G(), *b = B();
  ByteImage img(nr, nc);

  for (int i = 0; i < nr * nc; i++)
    img.pixels[i] = (BYTE)((r[i] + g[i] + b[i]) / 3);

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
	img.at(r, c, ch) = (BYTE)(val >> 2);
      }	

  return img;
}

ByteImage::BYTE ByteImage::bilin(double r, double c, int ch) const {
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

  return (ByteImage::BYTE)(sum + 0.5);
}

ByteImage ByteImage::scale(double factor) const {
  ByteImage dest((int)(nr * factor + 0.5), (int)(nc * factor + 0.5), nchannels);
  
  factor = 1.0 / factor;
  for (int ch = 0; ch < dest.nchannels; ch++)
    for (int r = 0; r < dest.nr; r++)
      for (int c = 0; c < dest.nc; c++)
	dest.at(r, c, ch) = bilin(factor * r, factor * c, ch);

  return dest;
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
  pixels = new BYTE [size()];

  Magick::Pixels cache(img);
  Magick::PixelPacket* packet = cache.get(0, 0, nc, nr);

  //nshifts will allow a nice transition between quantizations
  int nshifts = 0;
  for (int i = MaxRGB >> 8; i & 1; i = i >> 1) nshifts++;
  
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++) {
      at(r, c, 0) = (BYTE)(packet[r * nc + c].red >> nshifts);
      at(r, c, 1) = (BYTE)(packet[r * nc + c].green >> nshifts);
      at(r, c, 2) = (BYTE)(packet[r * nc + c].blue >> nshifts);
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

//uf this can be optimized
void ByteImage::blit(const ByteImage& src, int destr, int destc) {
  if (src.nchannels != nchannels) {
    if (nchannels == 1) blit(src.toGrayscale(), destr, destc);
    else if (nchannels == 3) blit(src.toColor(), destr, destc);
    else printf("Can't blit: channel mismatch.\n");
    return;
  }

  int rd, cd;//Destination coordinates
  const BYTE *sr = src.R(), *sg = src.G(), *sb = src.B();
  BYTE *tr = R(), *tg = G(), *tb = B();

  for (int r = 0; r < src.nr; r++) {
    rd = destr + r;
    if (rd < 0) continue;
    else if (rd >= nr) break;

    for (int c = 0; c < src.nc; c++) {
      cd = destc + c;
      if (cd < 0) continue;
      else if (cd >= nc) break;
      
      tr[rd * nc + cd] = sr[r * src.nc + c];
      if (nchannels == 3) {
	tg[rd * nc + cd] = sg[r * src.nc + c];
	tb[rd * nc + cd] = sb[r * src.nc + c];
      }
    }
  }
}

#include <cmath>

void rgb2hsl(double r, double g, double b,
	     double &h, double &s, double &l) {
  //Get max and min of the rgb values
  double max = r;
  if (g > max) max = g; 
  if (b > max) max = b;

  double min = r;
  if (g < min) min = g; 
  if (b < min) min = b;
  
  double c = max - min;

  //Find hue in degrees
  if (c == 0) h = 0;
  else if (max == r) h = fmod((g - b) / c + 6, 6);
  else if (max == g) h = (b - r) / c + 2;
  else h = (r - g) / c + 4;

  h = 60 * h;
  
  //Find lightness
  l = 0.5 * (max + min);
  
  //Find saturation
  if (l > 0.0 && l < 1.0)
    s = c / (1 - fabs(2 * l - 1));
  else
    s = 0.0;
}

void hsl2rgb(double h, double s, double l,
	     double &r, double &g, double &b) {

  double c;
  if (l <= 0.5) c = 2 * l * s;
  else c = (2 - 2 * l) * s;

  double _h = h / 60.0;
  double x = c * (1 - fabs(fmod(_h, 2) - 1));

  switch ((int)_h) {
  default: r = 0; g = 0; b = 0; break;
  case 0: r = c; g = x; b = 0; break;
  case 1: r = x; g = c; b = 0; break;
  case 2: r = 0; g = c; b = x; break;
  case 3: r = 0; g = x; b = c; break;
  case 4: r = x; g = 0; b = c; break;
  case 5: r = c; g = 0; b = x; break;
  }    

  double m = l - 0.5 * c;
  r += m;
  g += m;
  b += m;
}

void ByteImage::setLightness(int r, int c, BYTE l) {
  double R = at(r, c, 0) / 255.0;
  double G = at(r, c, 1) / 255.0;
  double B = at(r, c, 2) / 255.0;
  double h, s, l0;

  rgb2hsl(R, G, B, h, s, l0);
  hsl2rgb(h, s, l / 255.0, R, G, B);

  at(r, c, 0) = (BYTE)(255.0 * R);
  at(r, c, 1) = (BYTE)(255.0 * G);
  at(r, c, 2) = (BYTE)(255.0 * B);  
}

ByteImage ByteImage::getLightness() const {
  if (nchannels != 3) return *this;

  ByteImage result(nr, nc);
  
  const ByteImage::BYTE *R = this->R(), *G = this->G(), *B = this->B();
  for (int i = 0; i < nr * nc; i++) {
    //Get max and min of the rgb values
    ByteImage::BYTE max = R[i];
    if (G[i] > max) max = G[i]; 
    if (B[i] > max) max = B[i];
    
    ByteImage::BYTE min = R[i];
    if (G[i] < min) min = G[i]; 
    if (B[i] < min) min = B[i];
    
    //Find lightness
    result[i] = ByteImage::avg(max, min);
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

void rgb2hsv(double r, double g, double b,
	     double& h, double& s, double& v) {
  //Get max and min of the rgb values
  double max = r;
  if (g > max) max = g; 
  if (b > max) max = b;

  double min = r;
  if (g < min) min = g; 
  if (b < min) min = b;
  
  double c = max - min;

  //Find hue in degrees
  if (c == 0) h = 0;
  else if (max == r) h = fmod((g - b) / c + 6, 6);
  else if (max == g) h = (b - r) / c + 2;
  else h = (r - g) / c + 4;

  h = 60 * h;
  
  //Find lightness
  v = max;
  
  //Find saturation
  if (max - min == 0) s = 0;
  else s = c / v;
}

void hsv2rgb(double h, double s, double v,
	     double& r, double& g, double& b) {

  double c = v * s;

  double _h = h / 60.0;
  double x = c * (1 - fabs(fmod(_h, 2) - 1));

  switch ((int)_h) {
  default: r = 0; g = 0; b = 0; break;
  case 0: r = c; g = x; b = 0; break;
  case 1: r = x; g = c; b = 0; break;
  case 2: r = 0; g = c; b = x; break;
  case 3: r = 0; g = x; b = c; break;
  case 4: r = x; g = 0; b = c; break;
  case 5: r = c; g = 0; b = x; break;
  }    

  double m = v - c;
  r += m;
  g += m;
  b += m;
}

ByteImage ByteImage::getValue() const {
  if (nchannels != 3) return *this;

  ByteImage result(nr, nc);
  
  const ByteImage::BYTE *R = this->R(), *G = this->G(), *B = this->B();
  for (int i = 0; i < nr * nc; i++) {
    //Get max of the rgb values
    ByteImage::BYTE max = R[i];
    if (G[i] > max) max = G[i]; 
    if (B[i] > max) max = B[i];
    
    //Find lightness
    result[i] = max;
  }

  return result;
}

void ByteImage::setValue(int r, int c, BYTE v) {
  double R = at(r, c, 0) / 255.0;
  double G = at(r, c, 1) / 255.0;
  double B = at(r, c, 2) / 255.0;
  double h, s, v0;

  rgb2hsv(R, G, B, h, s, v0);
  hsv2rgb(h, s, v / 255.0, R, G, B);

  at(r, c, 0) = (BYTE)(255.0 * R);
  at(r, c, 1) = (BYTE)(255.0 * G);
  at(r, c, 2) = (BYTE)(255.0 * B);  
}

ByteImage ByteImage::setValue(const ByteImage& V) const {
  if (nchannels == 1) return *this;
  
  ByteImage result(*this);
  int nr = (this->nr < V.nr)? this->nr : V.nr;
  int nc = (this->nc < V.nc)? this->nc : V.nc;
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++)
      result.setValue(r, c, V.at(r, c));
  return result;
}
