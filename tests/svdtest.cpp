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

void annihilate(Matrix& J, Matrix& U, Matrix& V, int p, int q, int k) {
  //TODO
}

void chase(Matrix& J, Matrix& U, Matrix& V, int p, int q) {
  const int m = J.rows(), n = J.cols();
  const int N = n - q;

  double w, th;
  Matrix S(m), T(n);
  
  //Compute Wilkinson shift and T2
  w = computeWilkinson(an2(J, N - 2) + bn2(J, N - 2), 
		       an(J, N - 2) * bn(J, N - 1), 
		       an2(J, N - 1) + bn2(J, N - 1));

  th = computeRotation(an2(J, p) - w, 
		       an(J, p) * bn(J, p + 1));
  T = Matrix::givens(n, p, p + 1, th);
  
  J = J * T;
  V = V * T;
  
  //Perform Givens rotation to chase the values down the off-bidiagonals
  for (int i = p; i < N - 1; i++) {
    if (i > p) {
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

/*
 * Compute singular value decomposition by GR-SVD
 * Matrix is bidiagonalized by Golub and Kahan,
 * then deflated by QR iteration.
 * Reference: Handbook of Linear Algebra, CRC press, 2007 ed.
 */
Matrix svd(const Matrix& A, Matrix& U, Matrix& V) {
  const double eps = DBL_EPSILON;
  const int m = A.rows(), n = A.cols();

  //Perform Golub-Kahan bidiagonalization
  Matrix J = A.bidiag(U, V);

  int i, p, q;//dimensions of square blocks B1,1 and B3,3 resp.
  for (;;) {
    /*
     * Deflate superdiagonal (beta) values
     */
    for (i = 0; i < n - 1; i++)
      if (dabs(bn(J, i + 1)) <= eps * (dabs(an(J, i)) + dabs(an(J, i + 1)))) {
	printf("Deflated beta %d\n", i + 1);
	bn(J, i + 1) = 0.0;
      }
    
    /*
     * Compute dimensions of blocks
     */
    
    //B3,3 (q x q) is the largest diagonal at the bottom-right of J
    for (q = 0; q < n - 1 && bn(J, n - q) == 0.0; q++);
    if (q == n - 1) break;//Convergence
    printf("q: %d\n", q);

    //B1,1 (p x p) is constructed so that B2,2 is the largest strictly bidiagonal block
    for (p = n - q; p > 0 && bn(J, p - 1) != 0.0; p--);
    if (p == 1) p = 0;
    printf("p: %d\n", p);

    printf("J so far:\n");
    printMatrix(J);
    pause();

    /*
     * Select rotation sequence
     */
    for (i = p; i < n - q; i++) {
      if (an(J, i) == 0.0) {
	annihilate(J, U, V, p, q, i);
	printMatrix(J);
	return J;
      }
    }
    if (i == n - q)
      chase(J, U, V, p, q);
  }
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
