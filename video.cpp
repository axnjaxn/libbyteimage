#include "video.h"

using namespace byteimage;

Video::Video() : src("") { }

Video::Video(int deviceid) {open_device(deviceid);}

Video::Video(std::string fn) {load_filename(fn);}

Video::Video(const Video& vid) {load_filename(vid.src);}

Video& Video::operator=(const Video& vid) {
  load_filename(vid.src);
  return *this;
}

void Video::load_filename(std::string fn) {
  src = fn;
  cap.open(fn);
  nc = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  nr = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  pos = (int)cap.get(CV_CAP_PROP_POS_FRAMES);
  frames = (int)cap.get(CV_CAP_PROP_FRAME_COUNT);
  fps = cap.get(CV_CAP_PROP_FPS);
}

void Video::open_device(int deviceid) {
  src = "";
  cap.open(deviceid);
  nc = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  nr = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  pos = frames = 0;
  fps = 30;
}

bool Video::nextFrame(ByteImage& I) {
  cv::Mat frame;
  if (!cap.read(frame)) return false;

  I = toByteImage(frame);

  return true;
}

VideoWriter::VideoWriter() { }

VideoWriter::VideoWriter(std::string fn, int nr, int nc, double fps) {
  open(fn, nr, nc, fps);
}

void VideoWriter::open(std::string fn, int nr, int nc, double fps) {
  int i = fn.size() - 4;
  if (fn[i] == '.' && fn[i + 1] == 'm' && fn[i + 2] == 'p' && fn[i + 3] == '4')
    writer.open(fn, CV_FOURCC('H', '2', '6', '4'), fps, cv::Size(nc, nr));
  else
    writer.open(fn, CV_FOURCC('D', 'I', 'V', 'X'), fps, cv::Size(nc, nr));
}

void VideoWriter::write(const ByteImage& image) {
  if (!writer.isOpened()) return;

  writer.write(toMat(image));
}
