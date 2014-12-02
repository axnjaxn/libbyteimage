/*
 * kernel.h by Brian Jackson
 * Revised 10 May 2014
 */

#ifndef _BPJ_BYTEIMAGE_KERNEL_H
#define _BPJ_BYTEIMAGE_KERNEL_H

#include "byteimage.h"

class Kernel {
public:
  int nr, nc;
  double* values;
  
  Kernel();
  Kernel(int nr, int nc);
  Kernel(const Kernel& k);
  ~Kernel();

  Kernel& operator=(const Kernel& k);

  inline double& at(int i, int j) {return values[(i + nr / 2) * nc + (j + nc / 2)];}
  inline double at(int i, int j) const {return values[(i + nr / 2) * nc + (j + nc / 2)];}

  double boundedAt(int i, int j) const;

  Kernel transpose() const;
  ByteImage convolve(const ByteImage& I, double factor = 1.0) const;
  double convolveAt(const ByteImage& I, int r, int c, int ch = 0) const;

  void print();

  static Kernel Identity();
  static Kernel BoxBlur(int w = 3);
  static Kernel SobelX();
  static Kernel SobelY();
  static Kernel Gaussian(double sigma);
  static Kernel LoG2D(double sigma);
};

#endif
