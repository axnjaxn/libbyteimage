#ifndef _BPJ_BYTEIMAGE_BYTEVIDEO_H
#define _BPJ_BYTEIMAGE_BYTEVIDEO_H

#include "byteimage.h"
#include <opencv2/opencv.hpp>

class ByteVideo {
protected:
  std::string src;
  cv::VideoCapture cap;
  int nr, nc, pos, frames;
  double fps;
  
public:
  ByteVideo();
  ByteVideo(std::string fn);
  ByteVideo(const ByteVideo& vid);
  
  ByteVideo& operator=(const ByteVideo& vid);

  void load_filename(std::string fn);

  inline int width() const {return nc;}
  inline int height() const {return nr;}
  inline int getIndex() const {return pos;}
  inline int count() const {return frames;}
  inline double FPS() const {return fps;}

  bool nextFrame(ByteImage& frame);
};

class ByteVideoWriter {
protected:
  cv::VideoWriter writer;

public:
  ByteVideoWriter();
  ByteVideoWriter(std::string fn, int nr, int nc, double fps = 30);
  ByteVideoWriter(const ByteVideoWriter&) = delete;

  ByteVideoWriter& operator=(const ByteVideoWriter&) = delete;

  void open(std::string fn, int nr, int nc, double fps = 30);
  void write(const ByteImage& image);
};


#endif
