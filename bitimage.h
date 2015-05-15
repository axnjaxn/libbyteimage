#ifndef _BPJ_BYTEIMAGE_BITIMAGE_H
#define _BPJ_BYTEIMAGE_BITIMAGE_H

#include "byteimage.h"

namespace byteimage {
  class BitImage {
  protected:
    int nstride;//In blocks

  public:
    typedef unsigned int Block;
    static const int BlockSize = sizeof(Block);
    static const int pos_bits;         //Computed in bitimage.cpp
    static const unsigned int bitmask; //Computed in bitimage.cpp

    Block* blocks;
    int nr, nc;
  
    BitImage();
    BitImage(int nr, int nc);
    BitImage(const BitImage& img);
    BitImage(BitImage&& img);
    virtual ~BitImage();

    BitImage& operator=(const BitImage& img);
    BitImage& operator=(BitImage&& img);

    inline int stride() const {return nstride;} //In blocks
    inline int size() const {return nr * nstride;} //In blocks
    inline int countBytes() const {return BlockSize * nr * nstride;}

    void clear();
    void resize(int nr, int nc);//Clears data

    inline bool at(int r, int c) const {return (blocks[r * nstride + (c >> pos_bits)] >> (c & bitmask)) & 0x01;}
    inline void set(int r, int c) {blocks[r * nstride + (c >> pos_bits)] |= 1 << (c & bitmask);}
    inline void unset(int r, int c) {blocks[r * nstride + (c >> pos_bits)] &= ~(1 << (c & bitmask));}
    inline void set(int r, int c, bool b) {if (b) set(r, c); else unset(r, c);}
  
    ByteImage toByteImage() const;

    void erode();
    void dilate();
    void open();
    void close();
    void invert();

    void AND(const BitImage& img);
    void OR(const BitImage& img);
    void XOR(const BitImage& img);

    static BitImage AND(const BitImage& img1, const BitImage& img2);
    static BitImage OR(const BitImage& img1, const BitImage& img2);
    static BitImage XOR(const BitImage& img1, const BitImage& img2);
    static BitImage threshold(const ByteImage& img, Byte t);//Sets bits for [t, 255]

    void fillHull();
  };
}

#endif
