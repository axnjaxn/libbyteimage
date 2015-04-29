#include "matrix.h"
#include <cstring>
#include <cmath>
#include <cfloat>

using namespace byteimage;

Matrix::Matrix() : nr(0), nc(0), data(nullptr) { }

Matrix::Matrix(int n) : nr(n), nc(n) {
  data = new double [nr * nc];
  memset(data, 0, nr * nc * sizeof(double));
}

Matrix::Matrix(int nr, int nc) : nr(nr), nc(nc) {
  data = new double [nr * nc];
  memset(data, 0, nr * nc * sizeof(double));
}

Matrix::Matrix(const double* ary, int nr, int nc) : nr(nr), nc(nc) {
  data = new double [nr * nc];
  memcpy(data, ary, nr * nc * sizeof(double));
}

Matrix::Matrix(std::initializer_list<double> v) : nr(v.size()), nc(1) {
  data = new double [nr];

  int i = 0;
  for (double d : v)
    data[i++] = d;
}

Matrix::Matrix(const Matrix& mat) : nr(0), nc(0), data(nullptr) {*this = mat;}

Matrix::Matrix(Matrix&& mat) : nr(0), nc(0), data(nullptr) {*this = mat;}

Matrix::~Matrix() {delete [] data;}

Matrix Matrix::identity(int n) {
  Matrix I(n);
  for (int i = 0; i < n; i++)
    I.at(i, i) = 1;
  return I;
}

Matrix Matrix::givens(int n, int i, int j, double th) {
  Matrix G(identity(n));
  double s = sin(th), c = cos(th);

  G.at(i, i) = G.at(j, j) = c;
  G.at(i, j) = -s;
  G.at(j, i) = s;

  return G;
}

Matrix& Matrix::operator=(const Matrix& mat) {
  if (nr * nc != mat.nr * mat.nc) {
    delete [] data;
    data = new double [mat.nr * mat.nc];
  }
  nr = mat.nr;
  nc = mat.nc;

  memcpy(data, mat.data, nr * nc * sizeof(double));

  return *this;
}

Matrix& Matrix::operator=(Matrix&& mat) {
  delete [] data;
  nr = mat.nr; mat.nr = 0;
  nc = mat.nc; mat.nc = 0;
  data = mat.data; mat.data = nullptr;

  return *this;
}

Matrix Matrix::operator+(const Matrix& mat) const {
  Matrix result(*this);
  for (int i = 0; i < size(); i++)
    result.data[i] += mat.data[i];
  return result;
}

Matrix Matrix::operator-(const Matrix& mat) const {
  Matrix result(*this);
  for (int i = 0; i < size(); i++)
    result.data[i] -= mat.data[i];
  return result;
}

Matrix Matrix::operator*(const Matrix& mat) const {
  Matrix result(nr, mat.nc);
  for (int r = 0; r < result.nr; r++)
    for (int c = 0; c < result.nc; c++)
      for (int i = 0; i < nc; i++)
	result.at(r, c) += at(r, i) * mat.at(i, c);
  return result;
}
  
Matrix Matrix::operator*(double d) const {
  Matrix result(*this);
  for (int i = 0; i < size(); i++)
    result.data[i] *= d;
  return result;
}

Matrix Matrix::operator/(double d) const {
  Matrix result(*this);
  for (int i = 0; i < size(); i++)
    result.data[i] /= d;
  return result;
}

Matrix Matrix::row(int i) const {
  Matrix result(1, nc);
  memcpy(result.data, data, nc * sizeof(double));
  return result;
}

Matrix Matrix::col(int i) const {
  Matrix result(nr, 1);
  for (int r = 0; r < nr; r++)
    result.data[r] = at(r, i);
  return result;
}

void Matrix::permuteRows(int i, int j) {
  if (i == j) return;

  double* temp = new double [nc];
  memcpy(temp, data + i * nc, nc * sizeof(double));
  memcpy(data + i * nc, data + j * nc, nc * sizeof(double));
  memcpy(data + j * nc, temp, nc * sizeof(double));
  delete [] temp;
}

Matrix Matrix::slice(int r0, int r1, int c0, int c1) const {
  Matrix s(r1 - r0 + 1, c1 - c0 + 1);
  for (int r = 0; r < s.nr; r++)
    memcpy(s.data + r * s.nc, data + (r0 + r) * nc + c0, s.nc * sizeof(double));
  return s;
}

void Matrix::replace(const Matrix& mat, int r0, int c0) {
  for (int r = 0; r < mat.nr; r++)
    memcpy(data + (r0 + r) * nc + c0, mat.data + r * mat.nc, mat.nc * sizeof(double));
}

Matrix Matrix::trans() const {
  Matrix result(nc, nr);
  for (int r = 0; r < result.nr; r++)
    for (int c = 0; c < result.nc; c++)
      result.at(r, c) = at(c, r);
  return result;
}

Matrix Matrix::inv() const {
  //TODO: Deal with non-square matrices
  if (nr != nc) return Matrix();
  return solve(*this, identity(nr));
}

inline static double dabs(double d) {return (d < 0.0)? -d : d;}

Matrix Matrix::solve(Matrix A, Matrix b) {
  double factor;

  for (int k = 0; k < A.nr; k++) {
    //Find highest pivot
    int highest = k;
    for (int i = k + 1; i < A.nr; i++)
      if (dabs(A.at(i, k)) > dabs(A.at(highest, k))) highest = i;
    A.permuteRows(highest, k);
    b.permuteRows(highest, k);

    //Scale row
    for (int i = k + 1; i < A.nr; i++) {
      if (A.at(k, k) == 0.0 || A.at(i, k) == 0.0) continue;
      factor = -A.at(i, k) / A.at(k, k);
      for (int j = k; j < A.nc; j++)
	A.at(i, j) += factor * A.at(k, j);
      for (int j = 0; j < b.nc; j++)
	b.at(i, j) += factor * b.at(k, j);
    }
  }

  //Perform back substitution
  for (int k = A.nr - 1; k >= 0; k--) {
    //Scale row k
    for (int c = 0; c < b.nc; c++)
      b.at(k, c) /= A.at(k, k);
    A.at(k, k) = 1.0;
   
    //Propagate upwards to clear column k
    for (int i = k - 1; i >= 0; i--) {
      if (A.at(i, k) == 0.0) continue;
      factor = -A.at(i, k);
      A.at(i, k) = 0.0;
      for (int c = 0; c < b.nc; c++)
	b.at(i, c) += factor * b.at(k, c);
    }
  }

  return b;
}

Matrix Matrix::solveNull(Matrix A) {
  //Row echelon form
  double factor;
  for (int k = 0; k < A.nr; k++) {
    //Find highest pivot
    int highest = k;
    for (int i = k + 1; i < A.nr; i++)
      if (dabs(A.at(i, k)) > dabs(A.at(highest, k))) highest = i;
    A.permuteRows(highest, k);

    //Scale row
    for (int i = k + 1; i < A.nr; i++) {
      if (A.at(k, k) == 0.0 || A.at(i, k) == 0.0) continue;
      factor = -A.at(i, k) / A.at(k, k);
      for (int j = k; j < A.nc; j++)
	A.at(i, j) += factor * A.at(k, j);
    }
  }

  //Basic vs free variables
  int* freevars = new int [A.nc], nfree = 0;
  for (int r = 0, c = 0; r < A.nr && c < A.nc; c++)
    if (A.at(r, c) == 0.0) freevars[nfree++] = c;
    else r++;

  //Reconstruct the matrix to allow fixing the free variables
  Matrix M(A.nc, A.nc), N(A.nc, nfree), zero(A.nc, 1);
  M.replace(A, 0, 0);
  for (int i = 0; i < nfree; i++)
    M.at(freevars[i], freevars[i]) = 1.0;

  //Fix each free variable and solve for the null vector bases
  for (int i = 0; i < nfree; i++) {
    zero.at(freevars[i]) = 1.0;
    N.replace(solve(M, zero), 0, i);
    zero.at(freevars[i]) = 0.0;
  }

  delete [] freevars;
  return N;
}

Matrix Matrix::cholesky() const {
  Matrix L(rows(), cols());

  double sum;
  for (int i = 0; i < L.rows(); i++) {
    for (int j = 0; j < i; j++) {
      sum = 0.0;
      for (int k = 0; k < j; k++)
	sum += L.at(i, k) * L.at(j, k);
      L.at(i, j) = (at(i, j) - sum) / L.at(j, j);
    }

    sum = 0.0;
    for (int k = 0; k < i; k++)
      sum += L.at(i, k) * L.at(i, k);
    
    L.at(i, i) = sqrt(at(i, i) - sum);
  }

  return L;
}

Matrix Matrix::bidiag(Matrix& P, Matrix& Q) const {
  const int m = nr, n = nc;

  Matrix u(m, 1), v(n, 1), w, AT = this->trans(), J(m, n);
  double alpha, beta;
  
  P = Matrix::identity(m);
  Q = Matrix::identity(n);
  v.at(0) = 1.0;
  beta = 0.0;

  for (int k = 0; k < n; k++) {
    w = (*this) * v - beta * u;
    alpha = length(w);
    if (alpha)
      u = w / alpha;
    else
      u = P.slice(0, m, k, k);

    w = AT * u - alpha * v;
    beta = length(w);
    if (beta)
      v = w / beta;
    else if (k < n - 1)
	v = Q.slice(0, n, k + 1, k + 1);

    P.replace(u, 0, k);
    J.at(k, k) = alpha;
    
    if (k < n - 1) {
      Q.replace(v, 0, k + 1);
      J.at(k, k + 1) = beta;
    }
  }

  return J;
}


Matrix byteimage::makePoint(double x, double y, double w) {
  Matrix v(3, 1); 
  v.at(0) = x; v.at(1) = y; v.at(2) = w;
  return v;
}

Matrix byteimage::normalize(const Matrix& v) {
  Matrix u(v);
  u.at(0) /= u.at(2);
  u.at(1) /= u.at(2);
  u.at(2) = 1.0;
  return u;
}

double byteimage::sqLength(const Matrix& v) {
  double sum = 0.0;
  for (int i = 0; i < v.rows(); i++)
    sum += v.at(i) * v.at(i);
  return sum;
}

double byteimage::length(const Matrix& v) {
  return sqrt(sqLength(v));
}

/*** SVD code begins here ***/

/*
 * Divide the matrix into three blocks:
 * [ B1,1   0   0   ] p rows
 * [   0  B2,2  0   ] n - q - p rows
 * [   0    0  B3,3 ] q rows
 * B3,3 is the largest strictly diagonal block, hence q is maximized
 * B2,2 is the largest strictly bidiagonal block, hence p is minimized
 * Each pass of the Golub-Reinsch QR operates on B2,2.
 */
static void pqBlock(const Matrix& J, int& p, int& q) {
  const int n = J.cols();

  //B3,3 (q x q) is the largest diagonal at the bottom-right of J
  for (q = 0; q < n - 1 && J.at(n - q - 2, n - q - 1) == 0.0; q++);
  if (q == n - 1) q = n;
  
  //B1,1 (p x p) is constructed so that B2,2 is the largest strictly bidiagonal block
  for (p = n - q - 1; p > 0 && J.at(p - 1, p) != 0.0; p--);
  if (p < 0) p = 0;
}

/*
 * Plug cosines and sines of an angle
 * into a Givens rotation matrix of size n
 * Note: passing in -sn instead of sn is equivalent to a transverse
 */
static Matrix givensCS(int n, int i, int j, double cs, double sn) {
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
static double rot(double a, double b, double& cs, double& sn) {
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
    r = a * tt;
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
static void eliminateBeta(Matrix& J, Matrix& U, Matrix& V, int p, int q, int k) {
  const int m = J.rows(), n = J.cols();
  const int N = n - q;

  double cs, sn;
  if (k + 1 < N) {
    Matrix S;
    for (int i = k + 1; i < N; i++) {
      rot(J.at(k, i), J.at(i, i), cs, sn);
      S = givensCS(m, k, i, sn, cs);
      J = S * J;
      U = U * S.trans();
    }
  }
  else {
    Matrix T;
    for (int i = k - 1; i >= 0; i--) {
      if (rot(J.at(i, i), J.at(i, k), cs, sn) < 0) {
	cs = -cs;
	sn = -sn;
      }
      T = givensCS(n, i, k, cs, sn);

      J = J * T;
      V = V * T;
    }
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
static double computeWilkinson(double a, double b, double c) {
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
static void chase(Matrix& J, Matrix& U, Matrix& V, int p, int q) {
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
}

static void swapColumns(Matrix& A, int i, int j) {
  double t;
  for (int k = 0; k < A.rows(); k++) {
    t = A.at(k, i);
    A.at(k, i) = A.at(k, j);
    A.at(k, j) = t;
  }
}

static void swapDiag(Matrix& A, int i, int j) {
  double t = A.at(i, i);
  A.at(i, i) = A.at(j, j);
  A.at(j, j) = t;
}

static void reorderSVD(Matrix& U, Matrix& W, Matrix& V) {
  const int m = U.rows(), n = V.rows();
  int i, j, k;

  for (i = 0; i < n - 1; i++) {
    k = i;
    for (j = i + 1; j < n; j++)
      if (dabs(W.at(j, j)) > dabs(W.at(k, k))) k = j;
    if (i != k) {
      swapColumns(U, i, k);
      swapDiag(W, i, k);
      swapColumns(V, i, k);
    }
  }
}

/*
 * Compute singular value decomposition by GR-SVD
 * Matrix is bidiagonalized by Golub and Kahan,
 * then deflated by QR iteration.
 * Reference: Handbook of Linear Algebra, CRC press, 2007 ed.
 */
Matrix Matrix::svd(Matrix& U, Matrix& V, bool reorder) const {
  const double eps = DBL_EPSILON;
  const int m = nr, n = nc;

  //Perform Golub-Kahan bidiagonalization
  Matrix J = bidiag(U, V);
  int i, p, q;
  while (true) { 
    //Test for beta-convergence
    for (i = 0; i < n - 1; i++)
      if (dabs(J.at(i, i + 1)) <= eps * (dabs(J.at(i, i)) + dabs(J.at(i + 1, i + 1))))
	J.at(i, i + 1) = 0.0;

    //Block the matrix into a diagonal, a strictly bidiagonal, and a remainder block
    pqBlock(J, p, q);

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

  if (reorder) reorderSVD(U, J, V);

  return J;
}

double byteimage::dot(const Matrix& v1, const Matrix& v2) {
  double sum = 0.0;
  for (int i = 0; i < v1.rows(); i++)
    sum += v1.at(i) * v2.at(i);
  return sum;
}
