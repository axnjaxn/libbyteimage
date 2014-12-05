#include "ransac.h"
#include <cmath>
#include <cstdlib>

template <typename tn>
inline void swap(tn& a, tn& b) {tn t = a; a = b; b = t;}

RANSAC::RANSAC(int subset_size, bool homogeneous) {
  this->subset_size = subset_size;
  this->homogeneous = homogeneous;
  p = 0.99;
  winit = 0.5;
  distance_threshold = 1.0;
}

double RANSAC::sqDist(const Matrix& a, const Matrix& b) const {
  int n = homogeneous? a.rows() - 1 : a.rows();
  double ssq = 0.0, d;

  if (homogeneous)
    for (int i = 0; i < n; i++) {
      d = a.at(i) / a.at(n) - b.at(i) / b.at(n);
      ssq += d * d;
    }
  else 
    for (int i = 0; i < n; i++) {
      d = a.at(i) - b.at(i);
      ssq += d * d;
    }
  
  return ssq;
}

Matrix RANSAC::findInliers(const std::vector<Matrix>& refp, 
			   const std::vector<Matrix>& tarp,
			   std::vector<int>& inliers) const {
  std::vector<int> allSelections(refp.size()), selections(subset_size);
  for (int i = 0; i < allSelections.size(); i++)
    allSelections[i] = i;

  std::vector<int> bestInliers;
  Matrix Tbest;

  unsigned int N = ~0;
  double eps = 1 - winit;

  int it;
  for (it = 0; it < N; it++) {
    //Select random subset
    for (int i = 0; i < subset_size; i++) {
      swap(allSelections[i], allSelections[i + rand() % (allSelections.size() - i)]);
      selections[i] = allSelections[i];
    }

    //Solve for the transformation
    Matrix T = solve(refp, tarp, selections);

    //Look for inliers
    inliers.clear();
    for (int i = 0; i < refp.size(); i++) {
      Matrix v = T * refp[i];
      if (sqDist(v, tarp[i]) <= distance_threshold * distance_threshold)
	inliers.push_back(i);
    }
      
    //If this result is better, replace it
    if (inliers.size() > bestInliers.size()) {
      bestInliers = inliers;
      Tbest = T;
    }

    //Set epsilon and N
    eps = 1 - (double)bestInliers.size() / refp.size();
    N = ceil(log(1 - p) / log(1 - pow((1 - eps), subset_size)));
  }

  inliers = bestInliers;
  return Tbest;
}

std::vector<Matrix> RANSAC::subset(const std::vector<Matrix>& vec, const std::vector<int>& selections) {
  std::vector<Matrix> s(selections.size());
  for (int i = 0; i < selections.size(); i++)
    s[i] = vec[selections[i]];
  return s;
}
