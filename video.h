#ifndef _BPJ_BYTEIMAGE_BYTEVIDEO_H
#define _BPJ_BYTEIMAGE_BYTEVIDEO_H

#include "byteimage.h"
#include <opencv2/opencv.hpp>

namespace byteimage {
  class Video {
  protected:
    std::string src;
    cv::VideoCapture cap;
    int nr, nc, pos, frames;
    double fps;
  
  public:
    Video();
    Video(std::string fn);
    Video(const Video& vid);
  
    Video& operator=(const Video& vid);

    void load_filename(std::string fn);

    inline int width() const {return nc;}
    inline int height() const {return nr;}
    inline int getIndex() const {return pos;}
    inline int count() const {return frames;}
    inline double FPS() const {return fps;}

    bool nextFrame(ByteImage& frame);
  };

  class VideoWriter {
  protected:
    cv::VideoWriter writer;

  public:
    VideoWriter();
    VideoWriter(std::string fn, int nr, int nc, double fps = 30);
    VideoWriter(const VideoWriter&) = delete;

    VideoWriter& operator=(const VideoWriter&) = delete;

    void open(std::string fn, int nr, int nc, double fps = 30);
    void write(const ByteImage& image);
  };
}


#endif
