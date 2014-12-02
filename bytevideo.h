/*
 * bytevideo.h by Brian Jackson
 * Revised 21 September 2012
 * Very minor update 2 December 2014
 * cflags must include `pkg-config opencv --cflags`
 * lflags must include `pkg-config opencv --libs`
 */

#ifndef _BPJ_DAGSI_BYTEVIDEO_H
#define _BPJ_DAGSI_BYTEVIDEO_H

#include "byteimage.h"
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

class ByteVideo {
protected:
  std::string src;
  CvCapture* cap;
  int nr, nc;
  
  static void toImage(IplImage* ipl, ByteImage& image);

public:
  ByteVideo();
  ByteVideo(std::string fn);
  ByteVideo(const ByteVideo& vid);
  virtual ~ByteVideo();
  
  ByteVideo& operator=(const ByteVideo& vid);

  void load_filename(std::string fn);

  inline int width() const {return nc;}
  inline int height() const {return nr;}

  bool nextFrame(ByteImage& frame);
  
  //Broken
  int getIndex() const;
  int count() const;
};

class ByteVideoWriter {
protected:
  CvVideoWriter* writer;

  static IplImage* toIplImage(const ByteImage& image);
  
public:
  ByteVideoWriter() {writer = NULL;}
  ByteVideoWriter(std::string fn, int nr, int nc, double fps = 24);

  void open(std::string fn, int nr, int nc, double fps = 24);

  void write(const ByteImage& image);

  void close();
};


#endif
