#include "hslimage.h"
#include <cstring>

using namespace byteimage;

HSLImage::HSLImage() : nr(0), nc(0), hsl(nullptr) { }

HSLImage::HSLImage(int nr, int nc) : nr(nr), nc(nc) {hsl = new float [3 * nr * nc];}

HSLImage::HSLImage(const ByteImage& img) : HSLImage(img.nr, img.nc) {
  const ByteImage::BYTE *R = img.R(), *G = img.G(), *B = img.B();
  for (int i = 0, j = 0; i < nr * nc; i++, j += 3)
    rgb2hsl(R[i], G[i], B[i], hsl[j], hsl[j + 1], hsl[j + 2]);
}

HSLImage::HSLImage(const HSLImage& img) : nr(0), nc(0), hsl(nullptr) {*this = img;}

HSLImage::HSLImage(HSLImage&& img) : nr(0), nc(0), hsl(nullptr) {*this = img;}

HSLImage::~HSLImage() {delete [] hsl;}

HSLImage& HSLImage::operator=(const HSLImage& img) {
  if (nr * nc != img.nr * img.nc) {
    delete [] hsl;
    hsl = new float [img.nr * img.nc];
  }
  
  nr = img.nr;
  nc = img.nc;
  memcpy(hsl, img.hsl, nr * nc * sizeof(float));

  return *this;
}

HSLImage& HSLImage::operator=(HSLImage&& img) {
  delete [] hsl;
  nr = img.nr; img.nr = 0;
  nc = img.nc; img.nc = 0;
  hsl = img.hsl; img.hsl = nullptr;
  return *this;
}

ByteImage HSLImage::toHue() {
  ByteImage img(nr, nc, 3);
  ByteImage::BYTE *R = img.R(), *G = img.G(), *B = img.B();
  for (int i = 0, j = 0; i < nr * nc; i++, j += 3)
    hsl2rgb(hsl[j], 1.0, 0.5, R[i], G[i], B[i]);
  return img;
}

ByteImage HSLImage::toSaturation() const {
  ByteImage img(nr, nc);
  for (int i = 0, j = 1; i < nr * nc; i++, j += 3)
    img[i] = (ByteImage::BYTE)(255.0 * hsl[j]);
  return img;
}

ByteImage HSLImage::toLightness() const {
  ByteImage img(nr, nc);
  for (int i = 0, j = 2; i < nr * nc; i++, j += 3)
    img[i] = (ByteImage::BYTE)(255.0 * hsl[j]);
  return img;
}

ByteImage HSLImage::toByteImage() const {
  ByteImage img(nr, nc, 3);
  ByteImage::BYTE *R = img.R(), *G = img.G(), *B = img.B();
  for (int i = 0, j = 0; i < nr * nc; i++, j += 3)
    hsl2rgb(hsl[j], hsl[j + 1], hsl[j + 2], R[i], G[i], B[i]);
  return img;
}

void HSLImage::panSharpen(const ByteImage& gray) {*this = panSharpened(gray);}

HSLImage HSLImage::panSharpened(const ByteImage& gray) const {
  //TODO
  return *this;
}

float byteimage::hueDistance(float h0, float h1) {
  h1 -= h0;
  if (h1 < 0.0) h1 += 360.0;
  if (h1 > 180.0) h1 = 360.0 - h1;
  return h1;
}
