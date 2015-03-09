#include <byteimage/byteimage_sdl2.h>
#include <byteimage/ransac.h>
#include <algorithm>

class MyRansac : public RANSAC {
protected:
  Matrix solve(const std::vector<Matrix>& refp, 
	       const std::vector<Matrix>& tarp,
	       const std::vector<int> selections) const {
    Matrix T = Matrix::identity(3), t;

    Matrix A(3);
    Matrix b(3);
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
	A.at(i, j) = refp[selections[i]].at(j);
    
    for (int i = 0; i < 3; i++)
      b.at(i) = tarp[selections[i]].at(0) / tarp[selections[i]].at(2);
    t = Matrix::solve(A, b);
    for (int i = 0; i < 3; i++)
      T.at(0, i) = t.at(i);

    for (int i = 0; i < 3; i++)
      b.at(i) = tarp[selections[i]].at(1) / tarp[selections[i]].at(2);
    t = Matrix::solve(A, b);
    for (int i = 0; i < 3; i++)
      T.at(1, i) = t.at(i);

    return T;
  }

public:
  MyRansac() : RANSAC(3, 1) {distance_threshold = 2.0;}
};

ByteImage drawPointPair(const std::vector<Matrix>& refp,
			const std::vector<Matrix>& tarp) {
  ByteImage img(256, 256);
  for (int i = 0; i < img.size(); i++) img[i] = 255;
  img = img.toColor();

  double x, y;
  int c, r;
  for (int i = 0; i < refp.size(); i++) {
    x = refp[i].at(0) / refp[i].at(2);
    y = refp[i].at(1) / refp[i].at(2);
    c = (int)(x + 0.5);
    r = (int)(y + 0.5);
    if (img.inBounds(r, c))
      img.at(r, c, 1) = img.at(r, c, 2) = 0;
  }

  for (int i = 0; i < tarp.size(); i++) {
    x = tarp[i].at(0) / tarp[i].at(2);
    y = tarp[i].at(1) / tarp[i].at(2);
    c = (int)(x + 0.5);
    r = (int)(y + 0.5);
    if (img.inBounds(r, c))
      img.at(r, c, 0) = 0;
  }

  return img.u2();
}

int main(int argc, char* argv[]) {
  std::vector<Matrix> refp, tarp;
  
  Matrix T = Matrix::identity(3);
  T.at(0, 0) = T.at(1, 1) = cos(0.1);
  T.at(1, 0) = sin(0.1);
  T.at(0, 1) = -T.at(1, 0);
  T.at(0, 2) = 5;
  T.at(1, 2) = 12;

  const int n = 100;
  for (int i = 0; i < n; i++) {
    Matrix m(3, 1);
    m.at(0) = rand() % 256;
    m.at(1) = rand() % 256;
    m.at(2) = 1.0;
    refp.push_back(m);

    m = T * m;
    m.at(0) /= m.at(2);
    m.at(1) /= m.at(2);
    m.at(2) = 1.0;
    tarp.push_back(m);
  }

  const int nbad = 20;
  for (int i = 0; i < nbad; i += 2)
    std::swap(tarp[i], tarp[i + 1]);
  
  display(drawPointPair(refp, tarp));

  std::vector<int> inliers;
  T = MyRansac().findInliers(refp, tarp, inliers);
  printf("%d inliers.\n", (int)inliers.size());

  for (int i = 0; i < n; i++)
    refp[i] = T * refp[i];

  display(drawPointPair(refp, tarp));
  
  return 0;
}
