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

#include <cmath>
#include <cfloat>

inline double dabs(double d) {return (d >= 0)? d : -d;}

/*
 * Derived from 8.11.2 of Datta
 */ 
double computeWilkinson(double a, double b, double c) {
  double r = (a - c) / 2;
  return c + dabs(b * b / (dabs(r) + sqrt(r * r + b * b)));
}

/*
 * Derived geometrically
 */
double computeRotation(double a, double b) {
  return -atan2(b, a);
}

inline double sq(double d) {return d * d;}
inline double& an(Matrix& J, int n) {return J.at(n, n);}
inline double& bn(Matrix& J, int n) {return J.at(n - 1, n);}
inline double an2(Matrix& J, int n) {return sq(an(J, n));}
inline double bn2(Matrix& J, int n) {return sq(bn(J, n));}

inline void pause() {
  getchar();
}

/*
 * Compute singular value decomposition by GR-SVD
 * Matrix is bidiagonalized by Golub and Kahan,
 * then deflated by QR iteration.
 * TODO: return notes
 */
Matrix svd(const Matrix& A, Matrix& U, Matrix& V) {
  const double eps = DBL_EPSILON;

  const int m = A.rows(), n = A.cols();

  Matrix J = A.bidiag(U, V);
  //At this step, U and V are equivalent to P and Q in GK bidiagonalization
  //P is m x m, J is m x n, Q is n x n

  printf("J initial:\n");
  printMatrix(J);

  //Note: in this section N can be considered the post-deflation rank of J
  double w, th;
  Matrix S(m), T(n);
  int iteration = 0;
  for (int N = J.cols(); N > 1;) {
    //Deflation check
    if (dabs(bn(J, N - 1)) <= 1e-5) {//eps * (dabs(an(J, N - 1)) + dabs(an(J, N - 2)))) {
      bn(J, N - 1) = 0.0;
      N--;
      continue;
    }
    
    //Compute Wilkinson shift and T2
    w = computeWilkinson(an2(J, N - 2) + bn2(J, N - 2), 
			 an(J, N - 2) * bn(J, N - 1), 
			 an2(J, N - 1) + bn2(J, N - 1));
    th = computeRotation(an2(J, 0) - w, 
			 an(J, 0) * bn(J, 1));
    T = Matrix::givens(n, 0, 1, th);

    J = J * T;
    V = V * T;

    //Perform Givens rotation to chase the values down the off-bidiagonals
    for (int i = 0; i < N - 1; i++) {
      if (i > 0) {
	//Compute Ti, annihilating values above superdiagonal values
	th = -computeRotation(J.at(i - 1, i), J.at(i - 1, i + 1));
	T = Matrix::givens(n, i, i + 1, th);
	
	J = J * T;
	V = V * T;
      }

      //Compute Si^T, annihilating subdiagonal values
      th = computeRotation(J.at(i, i), J.at(i + 1, i));
      S = Matrix::givens(m, i, i + 1, th);

      J = S * J;
      U = U * S.trans();
    }
  }

  return J;
}

/*
 * svdtest.cpp
 */

int main(int argc, char* argv[]) {
#if 0
  Matrix A(3, 2);
  A.at(0, 0) = 1.0; A.at(0, 1) = -2.0;
  A.at(1, 0) = 0.0; A.at(1, 1) = 1.0;
  A.at(2, 0) = 1.0; A.at(2, 1) = 0.0;
#else
  Matrix A(3, 3);
  A.at(0, 0) = 1; A.at(0, 1) = 2;
  A.at(1, 1) = 2; A.at(1, 2) = 3;
  A.at(2, 2) = 1;
#endif

  printf("A:\n");
  printMatrix(A);

  Matrix U, W, V;
  W = svd(A, U, V);

  printf("W:\n");
  printMatrix(W);

  printf("U:\n");
  printMatrix(U);

  printf("V:\n");
  printMatrix(V);

  printf("U W V*:\n");
  printMatrix(U * W * V.trans());

  return 0;
}
