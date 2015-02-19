#include "bitimage.h"
#include <cstring>

static inline int posBits(int BlockSize) {
  int bits = 3, block = BlockSize;
  while (block >>= 1) bits++;
  return bits;
}

const int BitImage::pos_bits = posBits(BitImage::BlockSize);
const unsigned int BitImage::bitmask = (BitImage::BlockSize << 3) - 1;

BitImage::BitImage() {
  blocks = NULL;
  nr = nc = nstride = 0;
}

BitImage::BitImage(int nr, int nc) {
  this->nr = nr;
  this->nc = nc;
  nstride = (nc / BlockSize) + !!(nc % BlockSize);
  blocks = new Block [size()];
  memset(blocks, 0, countBytes());
}

BitImage::BitImage(const BitImage& img) {
  blocks = NULL;
  *this = img;
}

BitImage::~BitImage() {
  if (blocks) delete [] blocks;
}

BitImage& BitImage::operator=(const BitImage& img) {
  if (size() != img.size()) {
    if (blocks) delete [] blocks;
    blocks = new Block [img.size()];    
  }
  
  nr = img.nr;
  nc = img.nc;
  nstride = img.nstride;
  memcpy(blocks, img.blocks, countBytes());
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

  if (blocks) delete [] blocks;
  blocks = result.blocks;
  result.blocks = NULL;
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

  if (blocks) delete [] blocks;
  blocks = result.blocks;
  result.blocks = NULL;
}

void BitImage::open() {
  dilate();
  erode();
}

void BitImage::close() {
  erode();
  dilate();
}

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
