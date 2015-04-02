#include "component.h"
#include <queue>

BitImage Component::static_marking;

void Component::getCenter(double& x, double& y) const {
  x = y = 0.0;
  for (int i = 0; i < size(); i++) {
    x += points[i].c;
    y += points[i].r;
  }
  x /= points.size();
  y /= points.size();
}

void Component::getBounds(int& x, int& y, int& w, int &h) const {
  int x1, y1;
  x = x1 = points[0].c; y = y1 = points[0].r;
  for (int i = 1; i < size(); i++) {
    if (points[i].c < x) x = points[i].c;
    else if (points[i].c > x1) x1 = points[i].c;
    if (points[i].r < y) y = points[i].r;
    else if (points[i].r > y1) y1 = points[i].r;
  }
  w = x1 - x + 1;
  h = y1 - y + 1;
}

template <bool eight_connected>
Component Component::getComponentAt(BitImage& marking, const Component::Pt& initial_point) {
  Component component;
  Component::Pt pt;
  std::queue<Component::Pt> q;
  q.push(initial_point);

  while (!q.empty()) {
    pt = q.front();
    q.pop();

    if (marking.at(pt.r, pt.c)) continue;

    marking.set(pt.r, pt.c);
    component.add(pt);
    if (eight_connected) {
      if (pt.r > 0) {
	if (pt.c > 0) q.push(Component::Pt(pt.r - 1, pt.c - 1));
	q.push(Component::Pt(pt.r - 1, pt.c));
	if (pt.c < marking.nc - 1) q.push(Component::Pt(pt.r - 1, pt.c + 1));
      }
      if (pt.c > 0) q.push(Component::Pt(pt.r, pt.c - 1));
      if (pt.c < marking.nc - 1) q.push(Component::Pt(pt.r, pt.c + 1));
      if (pt.r < marking.nr - 1) {
	if (pt.c > 0) q.push(Component::Pt(pt.r + 1, pt.c - 1));
	q.push(Component::Pt(pt.r + 1, pt.c));
	if (pt.c < marking.nc - 1) q.push(Component::Pt(pt.r + 1, pt.c + 1));
      }
    }
    else {
      if (pt.r > 0)
	q.push(Component::Pt(pt.r - 1, pt.c));
      if (pt.r < marking.nr - 1)
	q.push(Component::Pt(pt.r + 1, pt.c));
      if (pt.c > 0)
	q.push(Component::Pt(pt.r, pt.c - 1));
      if (pt.c < marking.nc - 1)
	q.push(Component::Pt(pt.r, pt.c + 1));
    }
  }

  return component;
}

Component Component::getComponentAt(BitImage& marking, const Pt& initial_point, Flags flags) {
  return (flags & EIGHT_CONNECTED)? getComponentAt<true>(marking, initial_point) : getComponentAt<false>(marking, initial_point);
}

std::vector<Component> Component::getComponents(const BitImage& img, Flags flags) {
  /*
   * This ugly block serves a purpose:
   * since BitImage checks to see if it can reuse memory during assignment,
   * static allocation can be quite fast, but not threadsafe.
   */

  BitImage dynamic_marking;
  BitImage& marking = (flags & DYNAMIC_ALLOC)? dynamic_marking : static_marking;
  marking = img;
  marking.invert();
  
  std::vector<Component> components;
  for (int r = 0; r < img.nr; r++)
    for (int c = 0; c < img.nc; c++)
      if (!marking.at(r, c))
	components.push_back(getComponentAt(marking, Pt(r, c), flags));
  
  return components;		
}

std::vector<Component> Component::getComponents(const BitImage& img, const std::vector<Pt>& pts, Flags flags) {
  BitImage dynamic_marking;
  BitImage& marking = (flags & DYNAMIC_ALLOC)? dynamic_marking : static_marking;
  marking = img;
  marking.invert();
  
  std::vector<Component> components;
  for (int i = 0; i < pts.size(); i++)
    if (!marking.at(pts[i].r, pts[i].c))
      components.push_back(getComponentAt(marking, pts[i], flags));
  
  return components;		
}

BitImage Component::toBitImage() const {
  int x, y, w, h;
  getBounds(x, y, w, h);
  
  BitImage img(h, w);
  for (auto pt : points)
    img.set(pt.r - y, pt.c - x);

  return img;
}

void Component::fillHull() {
  int x, y, w, h;
  getBounds(x, y, w, h);
  
  BitImage img(h, w);
  for (auto pt : points)
    img.set(pt.r - y, pt.c - x);

  img.fillHull();
  
  points.clear();
  for (int r = 0; r < img.nr; r++)
    for (int c = 0; c < img.nc; c++)
      if (img.at(r, c))
	points.push_back(Component::Pt(r + y, c + x));  
}
