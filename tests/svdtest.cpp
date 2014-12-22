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
 * Divide the matrix into three blocks:
 * [ B1,1   0   0   ] p rows
 * [   0  B2,2  0   ] n - q - p rows
 * [   0    0  B3,3 ] q rows
 * B3,3 is the largest strictly diagonal block, hence q is maximized
 * B2,2 is the largest strictly bidiagonal block, hence p is minimized
 * Each pass of the Golub-Reinsch QR operates on B2,2.
 */
void pqBlock(const Matrix& J, int& p, int& q) {
  const int n = J.cols();

  //B3,3 (q x q) is the largest diagonal at the bottom-right of J
  for (q = 0; q < n - 1 && J.at(n - q - 2, n - q - 1) == 0.0; q++);
  if (q == n - 1) q = n;
  
  //B1,1 (p x p) is constructed so that B2,2 is the largest strictly bidiagonal block
  for (p = n - q - 1; p > 0 && J.at(p - 1, p) != 0.0; p--);
}

/*
 * Plug cosines and sines of an angle
 * into a Givens rotation matrix of size n
 * Note: passing in -sn instead of sn is equivalent to a transverse
 */
Matrix givensCS(int n, int i, int j, double cs, double sn) {
  Matrix G(Matrix::identity(n));
  
  G.at(i, i) = G.at(j, j) = cs;
  G.at(i, j) = -sn;
  G.at(j, i) = sn;

  return G;
}

/*
 * Adapted from DK90:
 * Computes cosine and sine of an angle such that
 * the resultant rotation eliminates b. 
 * Returns the new value for a: sqrt(a^2 + b^2)
 */
double rot(double a, double b, double& cs, double& sn) {
  double t, tt, r;

  if (a == 0.0) {
    cs = 0.0;
    sn = 1.0;
    r = b;
  }
  else if (dabs(a) > dabs(b)) {
    t = b / a;
    tt = sqrt(1 + t * t);
    cs = 1.0 / tt;
    sn = t * cs;
    r - a * tt;
  }
  else {
    t = a / b;
    tt = sqrt(1 + t * t);
    sn = 1.0 / tt;
    cs = t * sn;
    r = b * tt;
  }

  return r;
}

/*
 * A series of rotations can be used to zero a row if its diagonal entry is zero
 * Golub and Van Loan 1996 gives the following method
 */
void eliminateBeta(Matrix& J, Matrix& U, Matrix& V, int p, int q, int k) {
  const int m = J.rows(), n = J.cols();

  printf("Eliminating beta.\n");
  printf("Initial:\n");
  printMatrix(J);
  pause();

  Matrix S;
  double cs, sn;
  if (k + 1 < n) {
    for (int i = k + 1; i < n; i++) {
      rot(J.at(k, i), J.at(i, i), cs, sn);
      S = givensCS(m, k, i, sn, cs);
      J = S * J;
      U = U * S.trans();

      printf("J:\n");
      printMatrix(J);
      pause();
    }
  }
  else {

  }
}

/*
 * Convenience functions
 * an is the nth diagonal, starting at 0
 * bn is the nth superdiagonal, so that an and bn are on the same column
 * (hence, b0 is undefined)
 */
inline double sq(double d) {return d * d;}
inline double& an(Matrix& J, int n) {return J.at(n, n);}
inline double& bn(Matrix& J, int n) {return J.at(n - 1, n);}
inline double an2(Matrix& J, int n) {return sq(an(J, n));}
inline double bn2(Matrix& J, int n) {return sq(bn(J, n));}

/*
 * Derived from 8.11.2 of Datta
 * This finds the eigenvalue of:
 * [ a b ]
 * [ b c ]
 * closer to c.
 */ 
double computeWilkinson(double a, double b, double c) {
  double r = (a - c) / 2;
  double s = (r >= 0)? 1.0 : -1.0;
  return c - s * b * b / (dabs(r) + sqrt(r * r + b * b)) ;
}

/*
 * "Chase the bulge" down J.
 * As a result of this method, J will remain bidiagonal,
 * but off-diagonals will be reduced, and U & V accumulate the rotations.
 * p and q are matrix dimensions of the block representation of J.
 */
void chase(Matrix& J, Matrix& U, Matrix& V, int p, int q) {
  const int m = J.rows(), n = J.cols();
  const int N = n - q;

  double w, cs, sn;
  Matrix S(m), T(n);
  
  //Compute Wilkinson shift and T2
  w = computeWilkinson(sq(J.at(N - 2, N - 2)) + sq(J.at(N - 2, N - 1)),
		       J.at(N - 2, N - 1) * J.at(N - 1, N - 1),
		       sq(J.at(N - 1, N - 1)));

  rot(an2(J, p) - w, an(J, p) * bn(J, p + 1), cs, sn);
  T = givensCS(n, p, p + 1, cs, sn);
  
  J = J * T;
  V = V * T;
  
  //Perform Givens rotation to chase the values down the off-bidiagonals
  for (int i = p; i < N - 1; i++) {
    if (i > p) {
      //Compute Ti, annihilating values above superdiagonal values
      rot(J.at(i - 1, i), J.at(i - 1, i + 1), cs, sn);
      T = givensCS(n, i, i + 1, cs, sn);

      J = J * T;
      V = V * T;
    }

    //Compute Si^T, annihilating subdiagonal values
    rot(J.at(i, i), J.at(i + 1, i), cs, sn);
    S = givensCS(m, i, i + 1, cs, -sn);

    J = S * J;
    U = U * S.trans();
  }

  printf("J post-chase:\n");
  printMatrix(J);
  pause();
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
  Matrix J = A;//.bidiag(U, V);
  int i, p, q;

  while (true) { 
    //Test for beta-convergence
    for (i = 0; i < n - 1; i++)
      if (dabs(J.at(i, i + 1)) <= eps * (dabs(J.at(i, i)) + dabs(J.at(i + 1, i + 1))))
	J.at(i, i + 1) = 0.0;

    //Block the matrix into a diagonal, a strictly bidiagonal, and a remainder block
    pqBlock(J, p, q);

    printf("J:\n");
    printMatrix(J);
    printf("p: %d q: %d\n", p, q);
    pause();

    //Test if the entire matrix is diagonal
    if (q == n) break;

    //Determine a set of rotations to perform
    for (i = p; i < n - q; i++)
      if (J.at(i, i) == 0.0) {
	eliminateBeta(J, U, V, p, q, i);//This set of rotations eliminates one superdiagonal value
	break;
      }
    if (i == n - q)
      chase(J, U, V, p, q);//This set of rotations decreases the superdiagonal values towards convergence
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
  Matrix A(5);
  for (int i = 0; i < 5; i++) {
    A.at(i, i) = i + 1;
    if (i < 4)
      A.at(i, i + 1) = i + 2;
  }
  A.at(1, 1) = 0.0;
#endif

  printf("A:\n");
  printMatrix(A);
  pause();

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
