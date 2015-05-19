#ifndef _BPJ_BYTEIMAGE_KERNEL_H
#define _BPJ_BYTEIMAGE_KERNEL_H

#include "byteimage.h"

namespace byteimage {
  class Kernel {
  public:
    int nr, nc;
    double* values;
  
    Kernel();
    Kernel(int nr, int nc);
    Kernel(const Kernel& k);
    Kernel(Kernel&& k);
    virtual ~Kernel();

    Kernel& operator=(const Kernel& k);
    Kernel& operator=(Kernel&& k);

    inline double& at(int i, int j) {return values[(i + nr / 2) * nc + (j + nc / 2)];}
    inline double at(int i, int j) const {return values[(i + nr / 2) * nc + (j + nc / 2)];}
    double boundedAt(int i, int j) const;
    
    double min() const;
    double max() const;

    Kernel transpose() const;
    
    double convolveAt(const ByteImage& I, int r, int c, int ch = 0) const;
    ByteImage convolve(const ByteImage& I) const;
    ByteImage convolve(const ByteImage& I, double factor) const;
    ByteImage convolveSeparable(const ByteImage& I) const;
    ByteImage convolveMagnitude(const ByteImage& I) const;

    void print();

    static Kernel Identity();
    static Kernel BoxBlur(int w = 3);
    static Kernel SobelX();
    static Kernel SobelY();
    static Kernel Gaussian(double sigma);
    static Kernel Gradient(double sigma);
    static Kernel LoG(double sigma);
    static Kernel LoG2D(double sigma);
  };
}

#endif
