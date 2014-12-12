#include "matrix.h"
#include <cstring>
#include <cmath>

Matrix::Matrix() {
  nr = nc = 0;
  data = NULL;
}

Matrix::Matrix(int n) {
  nr = nc = n;
  data = new double [nr * nc];
  memset(data, 0, nr * nc * sizeof(double));
}

Matrix::Matrix(int nr, int nc) {
  this->nr = nr;
  this->nc = nc;
  data = new double [nr * nc];
  memset(data, 0, nr * nc * sizeof(double));
}

Matrix::Matrix(const Matrix& mat) {
  nr = nc = 0;
  data = NULL;
  *this = mat;
}

Matrix::~Matrix() {
  if (data) delete [] data;
}

Matrix Matrix::identity(int n) {
  Matrix I(n);
  for (int i = 0; i < n; i++)
    I.at(i, i) = 1;
  return I;
}

Matrix& Matrix::operator=(const Matrix& mat) {
  if (nr * nc != mat.nr * mat.nc) {
    if (data) delete [] data;
    data = new double [mat.nr * mat.nc];
  }
  nr = mat.nr;
  nc = mat.nc;

  memcpy(data, mat.data, nr * nc * sizeof(double));

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

inline double dabs(double d) {return (d < 0.0)? -d : d;}

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

Matrix makePoint(double x, double y, double w) {
  Matrix v(3, 1); 
  v.at(0) = x; v.at(1) = y; v.at(2) = w;
  return v;
}

Matrix normalize(const Matrix& v) {
  Matrix u(v);
  u.at(0) /= u.at(2);
  u.at(1) /= u.at(2);
  u.at(2) = 1.0;
  return u;
}
