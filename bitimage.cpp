#include "bitimage.h"
#include <cstring>

static inline int posBits(int BlockSize) {
  int bits = 3, block = BlockSize;
  while (block >>= 1) bits++;
  return bits;
}

const int BitImage::pos_bits = posBits(BitImage::BlockSize);
const unsigned int BitImage::bitmask = (BitImage::BlockSize << 3) - 1;

BitImage::BitImage() : nr(0), nc(0), nstride(0), blocks(nullptr) { }

BitImage::BitImage(int nr, int nc) : nr(nr), nc(nc) {
  nstride = (nc / BlockSize) + !!(nc % BlockSize);
  blocks = new Block [size()];
  clear();
}

BitImage::BitImage(const BitImage& img) : blocks(nullptr) {*this = img;}

BitImage::BitImage(BitImage&& img) : blocks(nullptr) {*this = img;}

BitImage::~BitImage() {delete [] blocks;}

BitImage& BitImage::operator=(const BitImage& img) {
  if (size() != img.size()) {
    if (blocks) delete [] blocks;
    blocks = new Block [img.size()];    
  }
  
  nr = img.nr;
  nc = img.nc;
  nstride = img.nstride;
  memcpy(blocks, img.blocks, countBytes());

  return *this;
}

BitImage& BitImage::operator=(BitImage&& img) {
  delete [] blocks;

  nr = img.nr; img.nr = 0;
  nc = img.nc; img.nc = 0;
  nstride = img.nstride;
  blocks = img.blocks; img.blocks = nullptr;

  return *this;
}

void BitImage::clear() {
  memset(blocks, 0, countBytes());
}

void BitImage::resize(int nr, int nc) {
  if (this->nr * this->nc != nr * nc) {
    delete [] blocks;
    blocks = nullptr;
  }

  this->nr = nr;
  this->nc = nc;
  nstride = (nc / BlockSize) + !!(nc % BlockSize);
  if (!blocks) blocks = new Block [size()];
 
  clear();
}
  
ByteImage BitImage::toByteImage() const {
  ByteImage img(nr, nc);
  for (int r = 0; r < nr; r++)
    for (int c = 0; c < nc; c++)
      if (at(r, c)) img.at(r, c) = 0xFF;
  return img;
}

//TODO: Better boundary conditions, bitwise optimization

void BitImage::erode() {
  BitImage result(nr, nc);
  
  for (int r = 1; r < nr - 1; r++)
    for (int c = 1; c < nc - 1; c++)
      if (at(r - 1, c - 1) && at(r - 1, c) && at(r - 1, c + 1)
	  && at(r, c - 1) && at(r, c) && at(r, c + 1)
	  && at(r + 1, c - 1) && at(r + 1, c) && at(r + 1, c + 1))
	result.set(r, c);

  *this = std::move(result);
}

void BitImage::dilate() {
  BitImage result(nr, nc);
  
  for (int r = 1; r < nr - 1; r++)
    for (int c = 1; c < nc - 1; c++)
      if (at(r, c)) {
	result.set(r - 1, c - 1);
	result.set(r - 1, c);
	result.set(r - 1, c + 1);
	result.set(r, c - 1);
	result.set(r, c);
	result.set(r, c + 1);
	result.set(r + 1, c - 1);
	result.set(r + 1, c);
	result.set(r + 1, c + 1);
      }

  *this = std::move(result);
}

void BitImage::open() {dilate(); erode();}

void BitImage::close() {erode(); dilate();}

void BitImage::invert() {
  for (int i = 0; i < size(); i++)
    blocks[i] = ~blocks[i];
}

void BitImage::AND(const BitImage& img) {
  for (int i = 0; i < size(); i++)
    blocks[i] &= img.blocks[i];
}

void BitImage::OR(const BitImage& img) {
  for (int i = 0; i < size(); i++)
    blocks[i] |= img.blocks[i];
}

void BitImage::XOR(const BitImage& img) {
  for (int i = 0; i < size(); i++)
    blocks[i] ^= img.blocks[i];
}

BitImage BitImage::AND(const BitImage& img1, const BitImage& img2) {
  BitImage img(img1);
  img.AND(img2);
  return img;
}

BitImage BitImage::OR(const BitImage& img1, const BitImage& img2) {
  BitImage img(img1);
  img.OR(img2);
  return img;  
}

BitImage BitImage::XOR(const BitImage& img1, const BitImage& img2) {
  BitImage img(img1);
  img.XOR(img2);
  return img;
}

BitImage BitImage::threshold(const ByteImage& img, ByteImage::BYTE t) {
  if (img.nchannels != 1) return threshold(img.toGrayscale(), t);
  BitImage out(img.nr, img.nc);
  for (int r = 0; r < img.nr; r++)
    for (int c = 0; c < img.nc; c++)
      if (img.at(r, c) >= t) out.set(r, c);
  return out;
}

/*
 * Lattice convex hull method
 * A scanline-based method for finding the convex hull of points on a lattice (such as pixels)
 * Upper bound is around O(n log n), with n as the number of "on" pixels
 */
class LCH {
public:
  class Anchor {
  public:
    int r, c;
    Anchor() : r(0), c(0) { }
    Anchor(int r, int c) : r(r), c(c) { }
  };

  std::vector<Anchor> left, right;

  static float slope(const Anchor& a0, const Anchor& a1) {
    return (float)(a1.c - a0.c) / (a1.r - a0.r);
  }

protected:
  static void findRowBounds(const BitImage& dual, int r, int& L, int& R) {
    for (L = 0; L < dual.nc - 1 && !dual.at(r, L); L++);
    for (R = dual.nc - 1; R > L && !dual.at(r, R); R--);
  }

  static float lastSlope(const std::vector<Anchor>& v) {
    return slope(v[v.size() - 2], v[v.size() - 1]);
  }

  void insertLeft(int r, int L) {
    Anchor A(r, L);
    while (left.size() > 1 && slope(left[left.size() - 1], A) <= lastSlope(left))
      left.pop_back();
    left.push_back(A);
  }

  void insertRight(int r, int R) {
    Anchor A(r, R);
    while (right.size() > 1 && slope(right[right.size() - 1], A) >= lastSlope(right))
      right.pop_back();
    right.push_back(A);
  }

  //TODO: Optimize this
  static BitImage getDual(const BitImage& img) {
    BitImage dual(img.nr + 1, img.nc + 1);
    
    for (int r = 0; r < img.nr; r++)
      for (int c = 0; c < img.nc; c++)
	if (img.at(r, c)) {
	  dual.set(r, c);
	  dual.set(r, c + 1);
	  dual.set(r + 1, c);
	  dual.set(r + 1, c + 1);
	}
    
    return dual;
  }

public:
  LCH(const BitImage& img) {
    BitImage dual(getDual(img));

    int L, R;
    findRowBounds(dual, 0, L, R);
    left.push_back(Anchor(0, L));
    right.push_back(Anchor(0, R));

    for (int r = 1; r < dual.nr; r++) {
      findRowBounds(dual, r, L, R);
      insertLeft(r, L);
      insertRight(r, R);
    }
  }
};

void BitImage::fillHull() {
  const float EPS = 1e-9;
  LCH hull(*this);

  int Li = 0, Ri = 0, c0, c1;
  float sL, sR;

  clear();
  for (int r = 0; r < nr; r++) {
    if (r >= hull.left[Li].r) {
      Li++;
      sL = LCH::slope(hull.left[Li - 1], hull.left[Li]);
    }
    if (r >= hull.right[Ri].r) {
      Ri++;
      sR = LCH::slope(hull.right[Ri - 1], hull.right[Ri]);
    }
    
    c0 = (int)(hull.left[Li - 1].c + sL * (r - hull.left[Li - 1].r + 0.5) + 0.5 + EPS);
    c1 = (int)(hull.right[Ri - 1].c + sR * (r - hull.right[Ri - 1].r + 0.5) - 0.5 - EPS);

    for (int c = c0; c <= c1; c++)
      set(r, c);
  }
}
