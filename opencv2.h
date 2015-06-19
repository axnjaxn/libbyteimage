#ifndef _BPJ_BYTEIMAGE_OPENCV_H
#define _BPJ_BYTEIMAGE_OPENCV_H

#include "byteimage.h"
#include <opencv2/opencv.hpp>

namespace byteimage {
  cv::Mat toMat(const ByteImage& img);
  ByteImage toByteImage(const cv::Mat& mat);
}

#endif
