#include "opencv2.h"

using namespace byteimage;

cv::Mat byteimage::toMat(const ByteImage& img) {
  if (img.nchannels != 3) return toMat(img.toColor());
  
  std::vector<cv::Mat> mat(3);

  for (int ch = 0; ch < 3; ch++) {
    mat[2 - ch] = cv::Mat(img.nr, img.nc, CV_8UC1);
    for (int r = 0; r < img.nr; r++)
      for (int c = 0; c < img.nc; c++)
	mat[2 - ch].at<unsigned char>(r, c) = img.at(r, c, ch);
  }

  cv::Mat mimg;
  merge(mat, mimg);

  return mimg;
}

ByteImage byteimage::toByteImage(const cv::Mat& mat) {
  ByteImage img(mat.rows, mat.cols, 3);
  
  cv::Mat channels[3];
  cv::split(mat, channels);

  for (int ch = 0; ch < 3; ch++)
    for (int r = 0; r < img.nr; r++)
      for (int c = 0; c < img.nc; c++)
	img.at(r, c, ch) = channels[2 - ch].at<unsigned char>(r, c);

  return img;
}
