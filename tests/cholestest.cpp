#include <byteimage/matrix.h>
#include <cstdio>

using namespace byteimage;

void printMatrix(const Matrix& M) {
  for (int r = 0; r < M.rows(); r++) {
    for (int c = 0; c < M.cols(); c++)
      printf("%lf\t", M.at(r, c));
    printf("\n");
  }
}

int main(int argc, char* argv[]) {
  Matrix A(3);
  for (int r = 0; r < 3; r++)
    for (int c = 0; c < 3; c++)
      A.at(r, c) = 1;
  A.at(2, 0) = A.at(0, 2) = 3;
  A.at(0, 0) = A.at(2, 2) = 5;
  
  Matrix H = A.cholesky();

  printf("Matrix A:\n");
  printMatrix(A);

  printf("Matrix H:\n");
  printMatrix(H);

  printf("Matrix H H^T \n");
  printMatrix(H * H.trans());

  return 0;
}
