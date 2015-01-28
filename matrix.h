#ifndef _BPJ_MATRIX_H
#define _BPJ_MATRIX_H

class Matrix {
private:
  double* data;
  int nr, nc;

  void permuteRows(int i, int j);
  
public:
  Matrix();
  Matrix(int n);
  Matrix(int nr, int nc);
  Matrix(const Matrix& mat);
  ~Matrix();

  static Matrix identity(int n);
  static Matrix givens(int n, int i, int j, double th);

  Matrix operator-() const {return (*this) * -1.0;}

  Matrix& operator=(const Matrix& mat);
  Matrix operator+(const Matrix& mat) const;
  Matrix operator-(const Matrix& mat) const;
  Matrix operator*(const Matrix& mat) const;
  
  Matrix operator*(double d) const;
  inline friend Matrix operator*(double d, const Matrix& mat) {return mat * d;}
  Matrix operator/(double d) const;

  Matrix row(int i) const;
  Matrix col(int i) const;

  inline int rows() const {return nr;}
  inline int cols() const {return nc;}
  inline int size() const {return nr * nc;}
  inline double& at (int r, int c = 0) {return data[r * nc + c];}
  inline double at (int r, int c = 0) const {return data[r * nc + c];}
  inline double* getArray() {return data;}
  inline const double* getArray() const {return data;}

  Matrix slice(int r0, int r1, int c0, int c1) const;
  inline Matrix sliceRows(int r0, int r1) const {return slice(r0, r1, 0, nc - 1);}
  inline Matrix sliceRow(int r) const {return slice(r, r, 0, nc - 1);}
  inline Matrix sliceCols(int c0, int c1) const {return slice(0, nr - 1, c0, c1);}
  inline Matrix sliceCol(int c) const {return slice(0, nr - 1, c, c);}
  void replace(const Matrix& mat, int r0, int c0);

  Matrix trans() const;
  Matrix inv() const;
  static Matrix solve(Matrix A, Matrix b);
  static Matrix solveNull(Matrix A);//Columns of the returned result are the basis
  
  Matrix cholesky() const;
  Matrix bidiag(Matrix& P, Matrix& Q) const;
  Matrix svd(Matrix& U, Matrix& V, bool reorder = false) const;

  inline friend double getX(const Matrix& v) {return v.data[0] / v.data[2];}
  inline friend double getY(const Matrix& v) {return v.data[1] / v.data[2];}
};

Matrix makePoint(double x, double y, double w = 1.0);
Matrix normalize(const Matrix& v);
double sqLength(const Matrix& v);
double length(const Matrix& v);
double dot(const Matrix& v1, const Matrix& v2);

#endif

