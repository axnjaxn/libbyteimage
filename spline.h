#ifndef _BPJ_BYTEIMAGE_SPLINE_H
#define _BPJ_BYTEIMAGE_SPLINE_H

#include "matrix.h"
#include <vector>

namespace byteimage {
  class Spline {
  protected:
    std::vector<Matrix> d;//De Boor points
    std::vector<double> U;//Knot vector
  public:
    inline void addPoint(Matrix pt) {d.push_back(pt);}
    inline void addKnot(double u) {U.push_back(u);}

    inline Matrix& operator[](int i) {return d[i];}
    inline const Matrix& operator[](int i) const {return d[i];}
    inline int countPoints() const {return d.size();}
  
    inline double& getKnot(int i) {return U[i];}
    inline double getKnot(int i) const {return U[i];}
    inline int countKnots() const {return U.size();}

    inline int degree() const {return countKnots() - countPoints() + 1;}
    inline double domainStart() const {return U.at(degree() - 1);}
    inline double domainEnd() const {return U.at(U.size() - degree());}
    Matrix at(double u) const;

    static Spline makeSpline(int degree, std::initializer_list<Matrix> pts);
  };
}

#endif
