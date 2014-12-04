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

  Matrix& operator=(const Matrix& mat);
  Matrix operator+(const Matrix& mat) const;
  Matrix operator-(const Matrix& mat) const;
  Matrix operator*(const Matrix& mat) const;
  
  Matrix operator*(double d) const;
  inline friend Matrix operator*(double d, const Matrix& mat) {return mat * d;}

  Matrix row(int i) const;
  Matrix col(int i) const;

  inline int rows() const {return nr;}
  inline int cols() const {return nc;}
  inline int size() const {return nr * nc;}
  inline double& at (int r, int c = 0) {return data[r * nc + c];}
  inline double at (int r, int c = 0) const {return data[r * nc + c];}
  inline const double* getArray() const {return data;}

  Matrix trans() const;
  Matrix inv() const;
  static Matrix solve(Matrix A, Matrix b);
};

#endif

