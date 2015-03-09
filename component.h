#ifndef _BPJ_BYTEIMAGE_COMPONENT_H
#define _BPJ_BYTEIMAGE_COMPONENT_H

#include "bitimage.h"
#include <vector>

class Component {
public:
  class Pt {
  public:
    int r, c;
    constexpr Pt() : r(0), c(0) { }
    constexpr Pt(int r, int c) : r(r), c(c) { }
  };

  typedef unsigned char Flags;
  static constexpr Flags STATIC_ALLOC = 0;
  static constexpr Flags DYNAMIC_ALLOC = 1;

protected:
  static BitImage static_marking;
  static Component getComponentAt(BitImage& marking, const Pt& initial_point);

public:
  std::vector<Pt> points;

  inline void add(const Pt& pt) {points.push_back(pt);}

  inline Pt& operator[](int i) {return points[i];}
  inline const Pt& operator[](int i) const {return points[i];}

  inline int size() const {return points.size();}

  void getCenter(double& x, double& y);
  void getBounds(int& x, int& y, int& w, int &h);

  static std::vector<Component> getComponents(const BitImage& img, Flags flags = 0);
  static std::vector<Component> getComponents(const BitImage& img, const std::vector<Pt>& pts, Flags flags = 0);
};

#endif
