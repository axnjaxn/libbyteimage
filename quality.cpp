#include "quality.h"
#include <vector>
#include <cstring>
#include <cmath>

#define EPS 0.01

double* Entropy::pdf(const ByteImage& img, const Window& w) const {
  double* p = new double [range()];
  memset(p, 0, range() * sizeof(double));
 
  for (int r = w.r; r < w.r + w.nr; r++)
    for (int c = w.c; c < w.c + w.nc; c++)
      p[img.at(r, c) >> nshifts]++;
  
  for (int i = 0; i < range(); i++)
    p[i] /= w.size();
  
  return p;
}

double* Entropy::pdf(const ByteImage& img0, const ByteImage& img1, const Window& w) const {
  double* p = new double [range() * range()];
  memset(p, 0, range() * range() * sizeof(double));
 
  int x, y;
  for (int r = w.r; r < w.r + w.nr; r++)
    for (int c = w.c; c < w.c + w.nc; c++) {
      x = img0.at(r, c) >> nshifts;
      y = img1.at(r, c) >> nshifts;
      p[x * range() + y]++;
    }
  
  for (int i = 0; i < range() * range(); i++)
    p[i] /= w.size();
  
  return p;
}

double* Entropy::pdf_x(double* joint) const {
  double* p = new double [range()];
  memset(p, 0, range() * sizeof(double));

  for (int x = 0; x < range(); x++)
    for (int y = 0; y < range(); y++)
      p[x] += joint[x * range() + y]; 

  return p;
}

double* Entropy::pdf_y(double* joint) const {
  double* p = new double [range()];
  memset(p, 0, range() * sizeof(double));

  for (int x = 0; x < range(); x++)
    for (int y = 0; y < range(); y++)
      p[y] += joint[x * range() + y];

  return p;
}

double Entropy::operator()(const ByteImage& img, const Window& w) const {
  if (img.nchannels > 1) return (*this)(img.toGrayscale(), w);

  double* p = pdf(img, w);
  double H = 0.0;
  for (int i = 0; i < range(); i++) {
    if (!p[i]) continue;
    H -= p[i] * log2(p[i]);
  }

  delete [] p;

  return H;
}

double Entropy::operator()(const ByteImage& img0, const ByteImage& img1, const Window& w) const {
  if (img0.nchannels > 1 || img1.nchannels > 1) return (*this)(img0.toGrayscale(), img1.toGrayscale(), w);

  double* p = pdf(img0, img1, w);
  double H = 0.0;
  for (int i = 0; i < range() * range(); i++) {
    if (!p[i]) continue;
    H -= p[i] * log2(p[i]);
  }

  delete [] p;

  return H;
}

#include <vector>
class SparseArray {
public:
  class Count {
  public:
    ByteImage::BYTE x, y, z;
    unsigned count;

    Count(ByteImage::BYTE x, ByteImage::BYTE y, ByteImage::BYTE z) {
      this->x = x; this->y = y; this->z = z;
      count = 1;
    }
  };
  std::vector<Count> v[256];
  
  void inc(ByteImage::BYTE x, ByteImage::BYTE y, ByteImage::BYTE z) {
    ByteImage::BYTE addr = x & y & z;
    int ind;
    for (ind = 0; ind < v[addr].size(); ind++)
      if (v[addr][ind].x == x && v[addr][ind].y == y && v[addr][ind].z == z) break;
    if (ind == v[addr].size())
      v[addr].push_back(Count(x, y, z));
    else
      v[addr][ind].count++;
  }
};

double Entropy::operator()(const ByteImage& img0, const ByteImage& img1, const ByteImage& img2, const Window& w) const {
  if (img0.nchannels > 1 || img1.nchannels > 1 || img2.nchannels > 1) return (*this)(img0.toGrayscale(), img1.toGrayscale(), img2.toGrayscale(), w);

  SparseArray array;
  ByteImage::BYTE x, y, z;
  for (int r = w.r; r < w.r + w.nr; r++)
    for (int c = w.c; c < w.c + w.nc; c++) {
      x = img0.at(r, c) >> nshifts;
      y = img1.at(r, c) >> nshifts;
      z = img2.at(r, c) >> nshifts;
      array.inc(x, y, z);
    }
  
  double p, H = 0.0;
  for (int addr = 0; addr < 256; addr++)
    for (int ind = 0; ind < array.v[addr].size(); ind++) {
      p = (double)array.v[addr][ind].count / w.size();
      H -= p * log2(p);
    }

  return H; 
}

double Entropy::cond(const ByteImage& img0, const ByteImage& img1, const Window& w) const {
  if (img0.nchannels > 1 || img1.nchannels > 1) return cond(img0.toGrayscale(), img1.toGrayscale(), w);

  double* pxy = pdf(img0, img1, w);
  double* py = pdf_y(pxy);

  double sum = 0.0;
  for (int x = 0; x < range(); x++)
    for (int y = 0; y < range(); y++) {
      if (!pxy[x * range() + y]) continue;
      sum += pxy[x * range() + y] * log2(py[y] / pxy[x * range() + y]);
    }

  delete [] pxy;
  delete [] py;

  return sum;
}

double Entropy::I(const ByteImage& img0, const ByteImage& img1, const Window& w) const {
  if (img0.nchannels > 1 || img1.nchannels > 1) return I(img0.toGrayscale(), img1.toGrayscale(), w);

  double* pxy = pdf(img0, img1, w);
  double* px = pdf_x(pxy);
  double* py = pdf_y(pxy);

  double sum = 0.0;
  for (int x = 0; x < range(); x++)
    for (int y = 0; y < range(); y++) {
      if (!pxy[x * range() + y]) continue;
      sum += pxy[x * range() + y] * log2(pxy[x * range() + y] / (px[x] * py[y]));
    }

  delete [] pxy;
  delete [] px;
  delete [] py;

  return sum;
}

double QualityMeasure::SSIM(const ByteImage& a, const ByteImage& b, const Window& w) const {
  if (a.nchannels > 1 || b.nchannels > 1)
    return SSIM(a.toGrayscale(), b.toGrayscale(), w);

  //Expected values
  double va, vb, Sa = 0.0, Sb = 0.0, Saa = 0.0, Sab = 0.0, Sbb = 0.0;
  const double c1 = 0.01, c2 = 0.01;

  for (int r = w.r; r < w.r + w.nr; r++)
    for (int c = w.c; c < w.c + w.nc; c++) {
      va = a.at(r, c);
      vb = b.at(r, c);
      Sa += va;
      Sb += vb;
      Saa += va * va;
      Sbb += vb * vb;
      Sab += va * vb;
    }

  const double mean_a = Sa / w.size();
  const double mean_b = Sb / w.size();
  const double var_a = Saa / w.size() - mean_a * mean_a;
  const double var_b = Sbb / w.size() - mean_b * mean_b;
  const double cov = Sab / w.size() - mean_a * mean_b;
  
  return ((2.0 * mean_a * mean_b + c1) / (mean_a * mean_a + mean_b * mean_b + c1)) * ((2.0 * cov + c2) / (var_a + var_b + c2));
}

double QualityMeasure::CAPT(const ByteImage& a, const ByteImage& b, const ByteImage& f, const Window& w) const {
  const double c = 0.01;

  double Ha = entropy(a, w);
  double Hab = entropy(a, b, w);
  double Haf = entropy(a, f, w);
  double Habf = entropy(a, b, f, w);
  
  return (Hab + Haf - Habf - Ha + c) / (Hab - Ha + c);
}

ByteImage QualityMeasure::visualizeQuality(const ByteImage& a, const ByteImage& b, const ByteImage& f) const {
  if (a.nchannels > 1 || b.nchannels > 1 || f.nchannels > 1)
    return visualizeQuality(a.toGrayscale(), b.toGrayscale(), f.toGrayscale());

  /* Determine windows */
  std::vector<Window> windows;
  {
    Window window(0, 0, window_size, window_size);
    for (window.r = (a.nr % window_size) / 2;
	 window.r + window.nr <= a.nr;
	 window.r += window.nr)
      for (window.c = (a.nc % window_size) / 2;
	   window.c + window.nc <= a.nc; 
	   window.c += window.nc)
	windows.push_back(window);
  }

  /* Compute saliency measure */
  std::vector<double> Sa(windows.size()), Sb(windows.size()), Sf(windows.size());
  for (int i = 0; i < windows.size(); i++) {
    Sa[i] = entropy(a, windows[i]);
    Sb[i] = entropy(b, windows[i]);
    Sf[i] = entropy(f, windows[i]);
  }

  /* Compute final quality measure map */
  ByteImage result(f.nr, f.nc);
  double Q, lambda;
  ByteImage::BYTE v;
  for (int i = 0; i < windows.size(); i++) {
    lambda = Sa[i] / (Sa[i] + Sb[i]);
    if (Sa[i] == 0.0 && Sb[i] == 0.0) lambda = 0.5;

    if (lambda < EPS)
      Q = SSIM(b, f, windows[i]);
    else if (1.0 - lambda < EPS)
      Q = SSIM(a, f, windows[i]);
    else
      Q = (lambda * SSIM(a, f, windows[i]) + (1.0 - lambda) * SSIM(b, f, windows[i])); 

    if (Q >= 1.0) v = 255;
    else if (Q <= 0.0) v = 0;
    else v = (unsigned char)(255.0 * Q);

    for (int r = 0; r < windows[i].nr; r++)
      for (int c = 0; c < windows[i].nc; c++)
	result.at(windows[i].r + r, windows[i].c + c) = v;
  }

  return result;
}

double QualityMeasure::Q_A(const ByteImage& a, const ByteImage& b, const ByteImage& f) const {
  if (a.nchannels > 1 || b.nchannels > 1 || f.nchannels > 1)
    return Q_A(a.toGrayscale(), b.toGrayscale(), f.toGrayscale());

  /* Determine windows */
  std::vector<Window> windows;
  {
    Window window(0, 0, window_size, window_size);
    for (window.r = (a.nr % window_size) / 2;
	 window.r + window.nr <= a.nr;
	 window.r += window.nr)
      for (window.c = (a.nc % window_size) / 2;
	   window.c + window.nc <= a.nc; 
	   window.c += window.nc)
	windows.push_back(window);
  }

  /* Compute saliency measure */
  std::vector<double> Sa(windows.size()), Sb(windows.size()), Sf(windows.size());
  for (int i = 0; i < windows.size(); i++) {
    Sa[i] = entropy(a, windows[i]);
    Sb[i] = entropy(b, windows[i]);
    Sf[i] = entropy(f, windows[i]);
  }

  /* Compute window weights */
  std::vector<double> C(windows.size());
  if (perceptual) {
    double sum = 0.0;
    for (int i = 0; i < C.size(); i++)
      sum += (C[i] = Sa[i] + Sb[i]);
    for (int i = 0; i < C.size(); i++)
      C[i] /= sum;
  }
  else {
    for (int i = 0; i < C.size(); i++)
      C[i] = 1.0 / C.size();
  }

  /* Compute final quality measure */
  const double c = 0.01, s2 = sqrt(2.0);
  double Q = 0.0, lambda;
  for (int i = 0; i < windows.size(); i++) {
    Q += C[i] * 0.5 * (SSIM(a, f, windows[i]) + CAPT(a, b, f, windows[i]));
  }

  return Q;
}

double QualityMeasure::operator()(const ByteImage& a, const ByteImage& b, const ByteImage& f) const {
  if (a.nchannels > 1 || b.nchannels > 1 || f.nchannels > 1)
    return (*this)(a.toGrayscale(), b.toGrayscale(), f.toGrayscale());

  /* Determine windows */
  std::vector<Window> windows;
  {
    Window window(0, 0, window_size, window_size);
    for (window.r = (a.nr % window_size) / 2;
	 window.r + window.nr <= a.nr;
	 window.r += window.nr)
      for (window.c = (a.nc % window_size) / 2;
	   window.c + window.nc <= a.nc; 
	   window.c += window.nc)
	windows.push_back(window);
  }

  /* Compute saliency measure */
  std::vector<double> Sa(windows.size()), Sb(windows.size()), Sf(windows.size());
  for (int i = 0; i < windows.size(); i++) {
    Sa[i] = entropy(a, windows[i]);
    Sb[i] = entropy(b, windows[i]);
    Sf[i] = entropy(f, windows[i]);
  }

  /* Compute window weights */
  std::vector<double> C(windows.size());
  if (perceptual) {
    double sum = 0.0;
    for (int i = 0; i < C.size(); i++)
      sum += (C[i] = Sa[i] + Sb[i]);
    for (int i = 0; i < C.size(); i++)
      C[i] /= sum;
  }
  else {
    for (int i = 0; i < C.size(); i++)
      C[i] = 1.0 / C.size();
  }

  /* Compute final quality measure */
  double Q = 0.0, lambda;
  double qa, qb;
  for (int i = 0; i < windows.size(); i++) {
    if (Sa[i] == 0.0 && Sb[i] == 0.0) continue;

    lambda = Sa[i] / (Sa[i] + Sb[i]);

    qa = SSIM(a, f, windows[i]);
    qb = SSIM(b, f, windows[i]);

    Q += C[i] * (lambda * qa  + (1.0 - lambda) * qb);
  }

  return Q;
}
