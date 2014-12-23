/*
 * svd.h?
 */

#include <byteimage/matrix.h>

/*
 * svd.cpp?
 */

#include <cstdio>

void printMatrix(const Matrix& M) {
  for (int r = 0; r < M.rows(); r++) {
    for (int c = 0; c < M.cols(); c++)
      printf("%lf\t", M.at(r, c));
    printf("\n");
  }
}

inline void pause() {getchar();}

/*
 * svdtest.cpp
 */

int main(int argc, char* argv[]) {
  Matrix A(3, 2);
  A.at(0, 0) = 1.0; A.at(0, 1) = -2.0;
  A.at(1, 0) = 0.0; A.at(1, 1) = 1.0;
  A.at(2, 0) = 1.0; A.at(2, 1) = 0.0;

  printf("A:\n");
  printMatrix(A);

  Matrix U, W, V;
  W = A.svd(U, V, 1);

  printf("U:\n");
  printMatrix(U);

  printf("V:\n");
  printMatrix(V);

  printf("W:\n");
  printMatrix(W);

  printf("U W V*:\n");
  printMatrix(U * W * V.trans());

  return 0;
}
