/*
 * kernel.cpp by Brian Jackson
 * Revised 10 May 2014
 */

#include "kernel.h"

#include <cmath>
#include <cstring>

Kernel::Kernel() {nr = nc = 0; values = NULL;}

Kernel::Kernel(int nr, int nc) {
  this->nr = nr; 
  this->nc = nc;
  values = new double [nr * nc];
  memset(values, 0, nr * nc * sizeof(double));
}

Kernel::Kernel(const Kernel& k) {
  values = NULL;
  *this = k;
}

Kernel::~Kernel() {if (values) delete [] values;}

Kernel& Kernel::operator=(const Kernel& k) {
  if (values) delete [] values;
  nr = k.nr; 
  nc = k.nc;
  values = new double [nr * nc];
  memcpy(values, k.values, nr * nc * sizeof(double));
  return *this;
}

double Kernel::boundedAt(int i, int j) const {
  if (i + nr / 2 < 0 || i + nr / 2 >= nr
      || j + nc / 2 < 0 || j + nc / 2 >= nc) {
    return 0.0;
  }
  else return at(i, j);
}

Kernel Kernel::transpose() const {
  Kernel k(nc, nr);
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++)
      k.values[c * nr + r] = values[r * nc + c];
  return k;
}

ByteImage Kernel::convolve(const ByteImage& I, double factor) const{
  ByteImage O(I.nr, I.nc, I.nchannels);

  for (int ch = 0; ch < O.nchannels; ch++)
    for (int r = 0; r < O.nr; r++)
      for (int c = 0; c < O.nc; c++) {
	O.at(r, c, ch) = ByteImage::clip(convolveAt(I, r, c, ch) * factor);
      }

  return O;
}

double Kernel::convolveAt(const ByteImage& I, int r, int c, int ch) const {
  int r1, c1;
  double sum = 0.0;
  for (int i = -nr / 2; i <= nr / 2; i++) {
    r1 = r + i;
#ifdef CONVOLVE_NO_EDGES
    if (r1 < 0 || r1 >= I.nr) continue;
#else
    if (r1 < 0) r1 = 0;
    else if (r1 >= I.nr) r1 = I.nr - 1;
#endif
    for (int j = -nc / 2; j <= nc / 2; j++) {
      c1 = c + j;
#ifdef CONVOLVE_NO_EDGES
      if (c1 < 0 || c1 >= I.nc) continue;
#else
      if (c1 < 0) c1 = 0;
      else if (c1 >= I.nc) c1 = I.nc - 1;
#endif
      sum += I.at(r1, c1, ch) * at(i, j);
    }
  }
  return sum;
}

void Kernel::print() {
  for (int r = 0; r < nr; r++) {
    printf("[\t");
    for (int c = 0; c < nc; c++)
      printf("%f\t", values[r * nc + c]);
    printf("]\n");
  }
}

Kernel Kernel::Identity() {
  Kernel k(1, 1);
  k.values[0] = 1.0;
  return k;
}

Kernel Kernel::BoxBlur(int w) {
  Kernel k(w, w);
  for (int i = 0; i < w * w; i++)
    k.values[i] = 1.0 / (w * w);
  return k;
}

Kernel Kernel::SobelX() {
  Kernel k(3, 3);
  k.values[0] = k.values[6] = -1.0;
  k.values[3] = -2.0;
  k.values[2] = k.values[8] = 1.0;
  k.values[5] = 2.0;
  return k;
}
Kernel Kernel::SobelY() {
  return SobelX().transpose();
}

Kernel Kernel::Gaussian(double sigma) {
  const double pi = 3.14159265358979;
  int radius = ceil(3 * sigma);
  int diameter = 2 * radius + 1;
  double s2 = sigma * sigma;
  double fact = pow(2 * pi * s2, -0.5);

  Kernel k(1, diameter);
  for (int x = 0; x < radius; x++)
    k.at(0, x) = k.at(0, -x) = fact * exp(-0.5 * x * x / s2);
  return k;
}

Kernel Kernel::LoG2D(double sigma) {
  const double pi = 3.14159265358979;
  int radius = ceil(3 * sigma);
  int diameter = 2 * radius + 1;
  double s2 = sigma * sigma;
  double fact = -1.0 / (pi * s2 * s2);

  Kernel k(diameter, diameter);
  for (int y = 0; y < radius; y++)
    for (int x = 0; x < radius; x++)
      k.at(y, x) = k.at(y, -x) = k.at(-y, x) = k.at(-y, -x) = fact * (1.0 - (x * x + y * y) / (2 * s2)) * exp(-(x * x + y * y) / (2 * s2));
  return k;
}  
