#include <byteimage/byteimage_sdl2.h>
#include <byteimage/matrix.h>
#include <cmath>
#include <cstdio>

using namespace byteimage;

class Transformer : public ByteImageDisplay {
protected:
  Matrix T, R, X, Y;
  ByteImage ref;
  bool resample;

  virtual void handleEvent(SDL_Event event) {
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
	T = T * Y;
	resample = 1;
	break;
      case SDLK_DOWN:
	Y.at(1, 2) = -Y.at(1, 2);
	T = T * Y;
	Y.at(1, 2) = -Y.at(1, 2);
	resample = 1;
	break;
      case SDLK_LEFT:
	T = T * X;
	resample = 1;
	break;
      case SDLK_RIGHT:
	X.at(0, 2) = -X.at(0, 2);
	T = T * X;
	X.at(0, 2) = -X.at(0, 2);
	resample = 1;
	break;
      case SDLK_a:
	T = R * T;
	resample = 1;
	break;
      case SDLK_s:
	T = R.trans() * T;
	resample = 1;
	break;
      default: break;
      }
    }

    ByteImageDisplay::handleEvent(event);
  }

  virtual void update() {
    if (resample) {
      updateImage(transformedImage());
      resample = 0;
    }

    ByteImageDisplay::update();
  }

  ByteImage transformedImage() {
    Matrix M1 = Matrix::identity(3);
    M1.at(0, 2) = -ref.nc / 2.0;
    M1.at(1, 2) = -ref.nr / 2.0;

    Matrix M2 = Matrix::identity(3);
    M2.at(0, 2) = -M1.at(0, 2);
    M2.at(1, 2) = -M1.at(1, 2);

    Matrix T = M2 * this->T * M1;

    ByteImage tar = ref;
    Matrix v(3, 1);
    for (int r = 0; r < tar.nr; r++)
      for (int c = 0; c < tar.nc; c++) {
	v.at(0) = c;
	v.at(1) = r;
	v.at(2) = 1.0;
	v = T * v;
	tar.at(r, c) = ref.bilin(v.at(1) / v.at(2), v.at(0) / v.at(2));
      }

    return tar;
  }

public:
  Transformer() : ByteImageDisplay(256, 256, "Matrix Test") {
    T = R = X = Y = Matrix::identity(3);

    double d = 2.0;
    double th = 3.1415926535 * 0.01;
    R.at(0, 0) = R.at(1, 1) = cos(th);
    R.at(0, 1) = -sin(th);
    R.at(1, 0) = sin(th);
    X.at(0, 2) = d;
    Y.at(1, 2) = d;

    ref = ByteImage(256, 256);
    for (int i = 0; i < ref.size(); i++)
      ref[i] = ((i / 16) & 1)? 0 : 255;
    resample = 1;
  }
};

int main(int argc, char* argv[]) {
  Transformer().main();  
  
  return 0;
}
