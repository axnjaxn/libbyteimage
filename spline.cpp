#include "spline.h"
#include <algorithm>

Matrix Spline::at(double u) const {
  if (u < domainStart()) u = domainStart();
  else if (u > domainEnd()) u = domainEnd();

  const int D = countPoints(), K = countKnots(), n = degree();
  
  //Search for containing interval
  int I;
  for (I = n; I < K - n; I++)
    if (U[I] > u) break;
  I--;

  //Triangular coefficients
  std::vector<Matrix> d1(n + 1);
  std::copy(d.begin() + I - n + 1, d.begin() + I + 2, d1.begin());
  double den;
  for (int k = 1; k <= n; k++)
    for (int i = I + 1, j; i >= I - n + k + 1; i--) {
      den = U[i + n - k] - U[i - 1];
      j = i - I + n - 1;
      d1[j] = ((U[i + n - k] - u) / den) * d1[j - 1] + ((u - U[i - 1]) / den) * d1[j];
    }
      
  return d1[n];
}

Spline Spline::makeSpline(int degree, std::initializer_list<Matrix> pts) {
  Spline spline;

  //Construct the knots, evenly spaced
  spline.U = std::vector<double>(pts.size() + degree - 1);
  for (int i = 0; i < degree; i++) spline.U[i] = 0.0;
  for (int i = 0; i < degree; i++) spline.U[spline.U.size() - i - 1] = pts.size() - degree;
  for (int i = degree; i < spline.U.size() - degree; i++)
    spline.U[i] = i - degree + 1.0;

  spline.d = std::vector<Matrix>(pts.size());
  int i = 0;
  for (auto pt : pts)
    spline.d[i++] = pt;

  return spline;
}
