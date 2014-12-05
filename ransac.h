#ifndef _BPJ_RANSAC_H
#define _BPJ_RANSAC_H

#include "matrix.h"
#include <vector>

/*
 * Subclassing RANSAC:
 * Provide a solve function to go from subset to transformation
 * Optionally, provide a sqDist method for non-Euclidean metrics
 * Provide a constructor to set distance_threshold, subset_size, and homogeneous
 */
class RANSAC {
private:
  int subset_size;
  bool homogeneous;

protected:
  double p;
  double winit;
  double distance_threshold;

  //Solves for a transformation given a subset of reference and target points
  virtual Matrix solve(const std::vector<Matrix>& refp, 
		       const std::vector<Matrix>& tarp,
		       const std::vector<int> selections) const = 0;

  //Expects a column vector for both arguments
  virtual double sqDist(const Matrix& a, const Matrix& b) const;

  inline int getSubsetSize() const {return subset_size;}
  inline bool isHomogeneous() const {return homogeneous;}

public:
  RANSAC(int subset_size, bool homogeneous = 0);

  Matrix findInliers(const std::vector<Matrix>& refp, 
		     const std::vector<Matrix>& tarp,
		     std::vector<int>& inliers) const;

  static std::vector<Matrix> subset(const std::vector<Matrix>& vec, const std::vector<int>& selections);
};

#endif
