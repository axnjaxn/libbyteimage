#include <byteimage/matrix.h>
#include <cstdio>

void printMatrix(const Matrix& M) {
  for (int r = 0; r < M.rows(); r++) {
    for (int c = 0; c < M.cols(); c++)
      printf("%.1lf\t", M.at(r, c));
    printf("\n");
  }
}

inline void pause() {printf("Press any key...\n"); getchar();}

int main(int argc, char* argv[]) {
  Matrix A(4, 5);
  A.at(0, 0) = 1.0; A.at(0, 1) = 1.0; A.at(0, 2) = 0.0; A.at(0, 3) = 1.0; A.at(0, 4) = 5.0;
  A.at(1, 0) = 1.0; A.at(1, 1) = 0.0; A.at(1, 2) = 0.0; A.at(1, 3) = 2.0; A.at(1, 4) = 2.0;
  A.at(2, 0) = 0.0; A.at(2, 1) = 0.0; A.at(2, 2) = 1.0; A.at(2, 3) = 4.0; A.at(2, 4) = -1.0;
  //Bottom row zeroes

  printf("A:\n");
  printMatrix(A);

  Matrix N = Matrix::solveNull(A);
  printf("Null space:\n");
  printMatrix(N);

  return 0;
}
