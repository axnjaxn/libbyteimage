#include "kernel.h"

#include <cmath>
#include <cstring>

using namespace byteimage;

Kernel::Kernel() : nr(0), nc(0), values(nullptr) { }

Kernel::Kernel(int nr, int nc) : nr(nr), nc(nc) {
  values = new double [nr * nc];
  memset(values, 0, nr * nc * sizeof(double));
}

Kernel::Kernel(const Kernel& k) : nr(0), nc(0), values(nullptr) {*this = k;}

Kernel::Kernel(Kernel&& k) : nr(0), nc(0), values(nullptr) {*this = k;}

Kernel::~Kernel() {delete [] values;}

Kernel& Kernel::operator=(const Kernel& k) {
  if (nr * nc != k.nr * k.nc) {
    delete [] values;
    values = new double [k.nr * k.nc];
  }
  nr = k.nr;
  nc = k.nc;

  memcpy(values, k.values, nr * nc * sizeof(double));

  return *this;
}

Kernel& Kernel::operator=(Kernel&& k) {
  nr = k.nr; k.nr = 0;
  nc = k.nc; k.nc = 0;
  values = k.values; k.values = nullptr;

  return *this;
}

double Kernel::boundedAt(int i, int j) const {
  if (i + nr / 2 < 0 || i + nr / 2 >= nr
      || j + nc / 2 < 0 || j + nc / 2 >= nc) {
    return 0.0;
  }
  else return at(i, j);
}

double Kernel::min() const {
  double sum = 0.0;
  for (int i = 0; i < nr * nc; i++)
    if (values[i] < 0.0) sum += values[i];
  return sum;
}

double Kernel::max() const {
  double sum = 0.0;
  for (int i = 0; i < nr * nc; i++)
    if (values[i] > 0.0) sum += values[i];
  return sum;  
}

Kernel Kernel::transpose() const {
  Kernel k(nc, nr);
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++)
      k.values[c * nr + r] = values[r * nc + c];
  return k;
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

ByteImage Kernel::convolve(const ByteImage& I) const {
  ByteImage O(I.nr, I.nc, I.nchannels);

  double min = this->min();
  double scale = this->max() - min;
  
  for (int ch = 0; ch < O.nchannels; ch++)
    for (int r = 0; r < O.nr; r++)
      for (int c = 0; c < O.nc; c++)
	O.at(r, c, ch) = clip((convolveAt(I, r, c, ch) - min) / scale);

  return O;
}

ByteImage Kernel::convolve(const ByteImage& I, double factor) const{
  ByteImage O(I.nr, I.nc, I.nchannels);

  for (int ch = 0; ch < O.nchannels; ch++)
    for (int r = 0; r < O.nr; r++)
      for (int c = 0; c < O.nc; c++)
	O.at(r, c, ch) = clip(convolveAt(I, r, c, ch) * factor);

  return O;
}

ByteImage Kernel::convolveSeparable(const ByteImage& I) const {
  return convolve(transpose().convolve(I));
}

ByteImage Kernel::convolveMagnitude(const ByteImage& I) const {
  ByteImage O(I.nr, I.nc, I.nchannels);

  Kernel T = transpose();
  
  const double C = 1.0 / sqrt(2);
  double min = this->min();
  double scale = this->max() - min;
  double X, Y;
  
  for (int ch = 0; ch < O.nchannels; ch++)
    for (int r = 0; r < O.nr; r++)
      for (int c = 0; c < O.nc; c++) {
	X = (convolveAt(I, r, c, ch) - min) / scale;
	Y = (T.convolveAt(I, r, c, ch) - min) / scale;
	O.at(r, c, ch) = clip(C * sqrt(X * X + Y * Y));
      }

  return O;
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
  int radius = (int)ceil(2 * sigma * sqrt(2.0 * log(10)));
  int diameter = 2 * radius + 1;
  double s2 = sigma * sigma;
  double s2inv = 1.0 / s2;
  double fact = pow(2 * pi * s2, -0.5);

  Kernel k(1, diameter);
  for (int x = 0; x <= radius; x++)
    k.at(0, x) = k.at(0, -x) = fact * exp(-0.5 * x * x * s2inv);
  return k;
}

Kernel Kernel::Gradient(double sigma) {
  Kernel k = Gaussian(sigma);
  int radius = k.nc / 2;
  double s2inv = 1.0 / (sigma * sigma);

  for (int x = -radius; x <= radius; x++)
    k.at(0, x) = -x * s2inv * k.at(0, x);

  return k;
}

Kernel Kernel::LoG(double sigma) {
  Kernel k = Gaussian(sigma);
  int radius = k.nc / 2;
  double s2inv = 1.0 / (sigma * sigma);

  for (int x = 0; x <= radius; x++)
    k.at(0, -x) = k.at(0, x) = s2inv * (x * x * s2inv - 1) * k.at(0, x);

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
