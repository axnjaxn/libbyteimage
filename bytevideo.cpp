#include "bytevideo.h"

void ByteVideo::toImage(IplImage* ipl, ByteImage& image) {
  if (image.nc != ipl->width || image.nr != ipl->height || image.nchannels != 3)
    image = ByteImage(ipl->height, ipl->width, 3);

  if (ipl->nChannels != 3 || ipl->dataOrder != 0) {
    printf("%d channels, order %d\n", ipl->nChannels, ipl->dataOrder);
    exit(2);
  }

  ByteImage::BYTE *R = image.pixels;
  ByteImage::BYTE *G = image.pixels + image.nr * image.nc;
  ByteImage::BYTE *B = image.pixels + 2 * image.nr * image.nc;
  char* row = NULL;
  for (int r = 0, c; r < image.nr; r++) {
    row = ipl->imageData + r * ipl->widthStep;
    for (c = 0; c < image.nc; c++) {
      R[r * image.nc + c] = row[3 * c + 2];
      G[r * image.nc + c] = row[3 * c + 1];
      B[r * image.nc + c] = row[3 * c];
    }
  }
}

ByteVideo::ByteVideo() {
  src = ""; 
  cap = NULL;
}

ByteVideo::ByteVideo(std::string fn) {
  cap = NULL;
  load_filename(fn);
}

ByteVideo::ByteVideo(const ByteVideo& vid) {
  cap = NULL;
  load_filename(vid.src);
}

ByteVideo::~ByteVideo() {
  cvReleaseCapture(&cap);
}

ByteVideo& ByteVideo::operator=(const ByteVideo& vid) {
  load_filename(vid.src);
  return *this;
}

void ByteVideo::load_filename(std::string fn) {
  if (cap) cvReleaseCapture(&cap);
  src = fn;
  cap = cvCreateFileCapture(fn.c_str());
  cvQueryFrame(cap);
  nc = cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH);
  nr = cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT);
}

bool ByteVideo::nextFrame(ByteImage& I) {
  if (cvGrabFrame(cap)) {
    toImage(cvRetrieveFrame(cap), I);
    return 1;
  }
  else return 0;
}

int ByteVideo::getIndex() const {
  return (int)cvGetCaptureProperty(cap, CV_CAP_PROP_POS_FRAMES);
}

int ByteVideo::count() const {
  return (int)cvGetCaptureProperty(cap, CV_CAP_PROP_FRAME_COUNT);
}

IplImage* ByteVideoWriter::toIplImage(const ByteImage& image) {
  return NULL;
  /*
  IplImage img;
  img.nsize = sizeof(IplImage);
  img.ID = 0;
  img.nChannels = 3;
  img.depth = IPL_DEPTH_8U;
  img.dataOrder = 1;
  img.origin = 0;
  img.width = image.nc;
  img.height = image.nr;
  img.roi = NULL;
  img.maskROI = NULL;
  img.imageID = NULL;
  img.tileInfo = NULL;
  img.widthStep = 3 * image.nc;
  img.imageSize = img.height * img.widthStep;
  
  uint8_t* data = new ;

  for (int ch = 0; ch < 3; ch++)
    for (int r = 0; r < image.nr; r++)
      for (int c = 0; c < image.nc; c++)
	image.at(r, c, ch) = ipl->imageData[r * ipl->widthStep + 3 * c + (2 - ch)];
  
  img.imageData = img.imageDataOrigin = data;
  
  return img;
  */
}

/*
cv::Mat toMat(const ByteImage& image) {
  for (int ch = 0; ch < 3; ch++)
    for (int r = 0; r < image.nr; r++)
      for (int c = 0; c < image.nc; c++)
	image.at(r, c, ch) = ipl->imageData[r * ipl->widthStep + 3 * c + (2 - ch)];
}
*/

ByteVideoWriter::ByteVideoWriter(std::string fn, int nr, int nc, double fps) {
  open(fn, nr, nc, fps);
}

void ByteVideoWriter::open(std::string fn, int nr, int nc, double fps) {
  int i = fn.size() - 4;
  if (fn[i] == '.' && fn[i + 1] == 'm' && fn[i + 2] == 'p' && fn[i + 3] == '4')
    writer = cvCreateVideoWriter(fn.c_str(), CV_FOURCC('H', '2', '6', '4'), fps, cvSize(nc, nr));
  else
    writer = cvCreateVideoWriter(fn.c_str(), CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(nc, nr));
}

void ByteVideoWriter::write(const ByteImage& image) {
  std::vector<cv::Mat> mat(3);

  //return;

  for (int ch = 0; ch < 3; ch++) {
    mat[2 - ch] = cv::Mat(image.nr, image.nc, CV_8UC1);
    for (int r = 0; r < image.nr; r++)
      for (int c = 0; c < image.nc; c++)
	mat[2 - ch].at<unsigned char>(r, c) = image.at(r, c, ch);
  }

  //return;

  cv::Mat mimg;
  merge(mat, mimg);

  //return;
  
  IplImage img = mimg;

  //return;

  cvWriteFrame(writer, &img);
}

void ByteVideoWriter::close() {
  if (writer) cvReleaseVideoWriter(&writer);
  writer = NULL;
}
