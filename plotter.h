#ifndef _BPJ_BYTEIMAGE_PLOTTER_H
#define _BPJ_BYTEIMAGE_PLOTTER_H

#include "matrix.h"
#include "font.h"

#include <string>
#include <vector>

class Plotter {
public:  
  //Defaults to black points, size 3, no line
  class Plot {
  public:
    std::vector<Matrix> points;//xyw
    Matrix color;//rgb
    int point_size, line_size;
    bool has_bar;

    Plot();

    void addPoint(double x, double y);
    void setColor(double r, double g, double b);
    void setPointSize(int size);
    void setLineSize(int size);
    void setBar(bool enabled);
  };

  TextRenderer *titleRender, *labelRender, *numberRender;
  int label_margin, title_margin, number_margin, border_margin;
  std::string xlabel, ylabel, title, xformat, yformat;

  Matrix mn, mx;//xy
  Matrix ticks, tick_offset;//xy
  bool draw_ticks, draw_grid;
  int axis_thickness, axis_offset;

  std::vector<Plot> plots;

  Plotter();
  virtual ~Plotter();

  void addPlot(const Plot& plot);
  void setScale(double xmin, double ymin, double xmax, double ymax);//Default: [0, 1]
  void hideTicks();//Default
  void setTicks(double xticks, double yticks, double xoffset, double yoffset);
  void enableGrid(bool enabled = 1);//Default: off
  void setTitleFont(const std::string& name, int size, int margin = 0);
  void setTitle(const std::string& title);
  void setLabelFont(const std::string& name, int size, int margin = 0);
  void setXLabel(const std::string& label);
  void setYLabel(const std::string& label);
  void setNumberFont(const std::string& name, int size, int margin = 0);
  void setNumberFormat(const std::string& xformat, const std::string& yformat);//Default: %.0lf, both visible (pass "" to hide)
  void setBorderMargin(int px);//Default: 5px
  void setAxisThickness(int px);//Default: 2px
  void setAxisOffset(int px);//Default: 3px

  ByteImage render(int nr, int nc) const;
};

#endif
