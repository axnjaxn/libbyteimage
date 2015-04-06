#include "bytevideo.h"

ByteVideo::ByteVideo() : src("") { }

ByteVideo::ByteVideo(std::string fn) {load_filename(fn);}

ByteVideo::ByteVideo(const ByteVideo& vid) {load_filename(vid.src);}

ByteVideo& ByteVideo::operator=(const ByteVideo& vid) {
  load_filename(vid.src);
  return *this;
}

void ByteVideo::load_filename(std::string fn) {
  src = fn;
  cap.open(fn);
  nc = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  nr = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  pos = (int)cap.get(CV_CAP_PROP_POS_FRAMES);
  frames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT);
  fps = cap.get(CV_CAP_PROP_FPS);
}

bool ByteVideo::nextFrame(ByteImage& I) {
  cv::Mat frame;
  if (!cap.read(frame)) return false;

  I = ByteImage(nr, nc, 3);
  cv::Mat channels[3];
  cv::split(frame, channels);
  for (int ch = 0; ch < 3; ch++)
    for (int r = 0; r < I.nr; r++)
      for (int c = 0; c < I.nc; c++)
	I.at(r, c, ch) = channels[2 - ch].at<unsigned char>(r, c);

  return true;
}

ByteVideoWriter::ByteVideoWriter() { }

ByteVideoWriter::ByteVideoWriter(std::string fn, int nr, int nc, double fps) {
  open(fn, nr, nc, fps);
}

void ByteVideoWriter::open(std::string fn, int nr, int nc, double fps) {
  int i = fn.size() - 4;
  if (fn[i] == '.' && fn[i + 1] == 'm' && fn[i + 2] == 'p' && fn[i + 3] == '4')
    writer.open(fn, CV_FOURCC('H', '2', '6', '4'), fps, cv::Size(nc, nr));
  else
    writer.open(fn, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(nc, nr));
}

void ByteVideoWriter::write(const ByteImage& image) {
  if (!writer.isOpened()) return;

  std::vector<cv::Mat> mat(3);

  for (int ch = 0; ch < 3; ch++) {
    mat[2 - ch] = cv::Mat(image.nr, image.nc, CV_8UC1);
    for (int r = 0; r < image.nr; r++)
      for (int c = 0; c < image.nc; c++)
	mat[2 - ch].at<unsigned char>(r, c) = image.at(r, c, ch);
  }

  cv::Mat mimg;
  merge(mat, mimg);

  writer.write(mimg);
}
