#ifndef _BPJ_FUSION_QUALITY_H
#define _BPJ_FUSION_QUALITY_H

#include "byteimage.h"

using namespace byteimage;//TODO

class Window {
public:
  int r, c, nr, nc;
  
  inline Window() {r = c = nr = nc = 0;}
  inline Window(int r, int c, int nr, int nc) {this->r = r; this->c = c; this->nr = nr; this->nc = nc;}
  inline Window(const ByteImage& img) {*this = Window(0, 0, img.nr, img.nc);}

  inline int size() const {return nr * nc;}
};

class Entropy {
protected:
  double* pdf(const ByteImage& img, const Window& w) const;
  double* pdf(const ByteImage& img0, const ByteImage& img1, const Window& w) const;
  double* pdf_x(double* joint) const;
  double* pdf_y(double* joint) const;

public:
  int nshifts;

  Entropy() {nshifts = 0;}

  //H(X)
  inline double operator()(const ByteImage& img) const {return (*this)(img, Window(img));}
  double operator()(const ByteImage& img, const Window& w) const;

  //H(X, Y)
  inline double operator()(const ByteImage& img0, const ByteImage& img1) const {return (*this)(img0, img1, Window(img0));}
  double operator()(const ByteImage& img0, const ByteImage& img1, const Window& w) const;

  //H(X, Y, Z)
  inline double operator()(const ByteImage& img0, const ByteImage& img1, const ByteImage& img2) const {return (*this)(img0, img1, img2, Window(img0));}
  double operator()(const ByteImage& img0, const ByteImage& img1, const ByteImage& img2, const Window& w) const;

  //H(X | Y)
  inline double cond(const ByteImage& img0, const ByteImage& img1) const {return cond(img0, img1, Window(img0));}
  double cond(const ByteImage& img0, const ByteImage& img1, const Window& w) const;

  //I(X; Y)
  inline double I(const ByteImage& img0, const ByteImage& img1) const {return I(img0, img1, Window(img0));}
  double I(const ByteImage& img0, const ByteImage& img1, const Window& w) const;

  inline int range() const {return (256 >> nshifts);}
};

class QualityMeasure {
public:
  Entropy entropy;
  int window_size;
  bool perceptual;

  QualityMeasure() {window_size = 32; perceptual = 1;}

  //Wang-Bovik quality measure (Q_0)
  inline double SSIM(const ByteImage& a, const ByteImage& b) const {return SSIM(a, b, Window(a));}
  double SSIM(const ByteImage& a, const ByteImage& b, const Window& w) const;
  double CAPT(const ByteImage& a, const ByteImage& b, const ByteImage& f, const Window& w) const;
  
  //Piella fusion quality measure
  ByteImage visualizeQuality(const ByteImage& a, const ByteImage& b, const ByteImage& f) const;
  double operator()(const ByteImage& a, const ByteImage& b, const ByteImage& f) const;

  //New quality measure
  double Q_A(const ByteImage& a, const ByteImage& b, const ByteImage& f) const;
};

#endif
