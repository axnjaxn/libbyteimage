#ifndef _BPJ_BYTEIMAGE_PLOTTER_H
#define _BPJ_BYTEIMAGE_PLOTTER_H

#include "font.h"

#include <string>
#include <vector>

namespace byteimage {
  class Plotter {
  public:  
    //Defaults to black points, size 3, no line
    class Plot {
    public:
      std::vector<Pt2f> points;
      Color color;
      int point_size, line_size;
      bool has_bar;

      Plot();

      void addPoint(double x, double y);
      void setColor(Byte r, Byte g, Byte b);
      void setPointSize(int size);
      void setLineSize(int size);
      void setBar(bool enabled);
    };

    TextRenderer *titleRender, *labelRender, *numberRender;
    int label_margin, title_margin, number_margin, border_margin;
    std::string xlabel, ylabel, title, xformat, yformat;

    Pt2f mn, mx, ticks, tick_offset;
    bool draw_ticks, draw_grid;
    int axis_thickness, axis_offset;

    std::vector<Plot> plots;

    Plotter();
    virtual ~Plotter();

    void addPlot(const Plot& plot);
    void setScale(double xmin, double ymin, double xmax, double ymax);//Default: [0, 1]
    void hideTicks();//Default
    void setTicks(double xticks, double yticks, double xoffset = 0.0, double yoffset = 0.0);
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
}

#endif
