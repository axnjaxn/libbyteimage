#include "plotter.h"
#include "render.h"

Plotter::Plot::Plot() {
  setSize(3);
  setPoint();
  setLine(0);
  setColor(0, 0, 0);
}

void Plotter::Plot::addPoint(double x, double y) {
  points.push_back(makePoint(x, y));
}

void Plotter::Plot::setColor(double r, double g, double b) {
  color = makeColor(0, 0, 0);
}

void Plotter::Plot::hideMarks() {
  has_bar = has_point = 0;
}

void Plotter::Plot::setPoint() {
  has_bar = 0;
  has_point = 1;
}

void Plotter::Plot::setBar() {
  has_bar = 1;
  has_point = 0;
}

void Plotter::Plot::setLine(bool enabled) {
  has_line = enabled;
}

void Plotter::Plot::setSize(int size) {
  sz = size;
}

Plotter::Plotter() {
  titleRender = labelRender = numberRender = NULL;
  label_margin = title_margin = number_margin = 0;
  setScale(0, 0, 1, 1);
  hideTicks();
  enableGrid(0);
  setNumberFormat("%.0lf", "%.0lf");
  setBorderMargin(5);
  setAxisThickness(2);
  setAxisOffset(3);
}

Plotter::~Plotter() {
  if (titleRender) delete titleRender;
  if (labelRender) delete labelRender;
  if (numberRender) delete numberRender;
}

void Plotter::addPlot(const Plot& plot) {
  plots.push_back(plot);
}

void Plotter::setScale(double xmin, double ymin, double xmax, double ymax) {
  mn = mx = Matrix(2, 1);
  mn.at(0) = xmin;
  mn.at(1) = ymin;
  mx.at(0) = xmax;
  mx.at(1) = ymax;
}

void Plotter::hideTicks() {
  draw_ticks = 0;
}

void Plotter::setTicks(double xticks, double yticks, double xoffset, double yoffset) {
  draw_ticks = 1;
  ticks = tick_offset = Matrix(2, 1);
  ticks.at(0) = xticks;
  ticks.at(1) = yticks;
  tick_offset.at(0) = xoffset;
  tick_offset.at(1) = yoffset;
}

void Plotter::enableGrid(bool enabled) {
  draw_grid = enabled;
}

void Plotter::setTitleFont(const std::string& name, int size, int margin) {
  if (titleRender) delete titleRender;
  titleRender = new TextRenderer(name.c_str(), size);
  title_margin = margin? margin : size * 3;
}

void Plotter::setTitle(const std::string& title) {
  this->title = title;
}

void Plotter::setLabelFont(const std::string& name, int size, int margin) {
  if (labelRender) delete labelRender;
  labelRender = new TextRenderer(name.c_str(), size);
  label_margin = margin? margin : size * 3;
}

void Plotter::setXLabel(const std::string& label) {
  xlabel = label;
}

void Plotter::setYLabel(const std::string& label) {
  ylabel = label;
}

void Plotter::setNumberFont(const std::string& name, int size, int margin) {
  if (numberRender) delete numberRender;
  numberRender = new TextRenderer(name.c_str(), size);
  number_margin = margin? margin : size * 3;
}

void Plotter::setNumberFormat(const std::string& xformat, const std::string& yformat) {
  this->xformat = xformat;
  this->yformat = yformat;
}

void Plotter::setBorderMargin(int px) {
  border_margin = px;
}

void Plotter::setAxisThickness(int px) {
  axis_thickness = px;
}

void Plotter::setAxisOffset(int px) {
  axis_offset = px;
}

ByteImage Plotter::render(int nr, int nc) const {
  ByteImage img(nr, nc, 3);
  memset(img.pixels, 0xFF, img.size());

  int x = 0, y = 0, w = nc, h = nr;

  //Compute border margin
  x += border_margin;
  y += border_margin;
  w -= 2 * border_margin;
  h -= 2 * border_margin;

  //Compute title area
  //TODO: Centering of text!
  if (titleRender && title_margin && !title.empty()) {
    //TODO
    h -= title_margin;
  }
  
  if (labelRender && label_margin) {
    //Compute xlabel
    if (!xlabel.empty()) {
      //TODO
      h -= label_margin;
    }

    //Compute ylabel
    if (!ylabel.empty()) {
      //TODO
      x += label_margin;
      w -= label_margin;
    }
  }

  //Compute numbers area
  if (numberRender && number_margin) {
    //Compute xnumbers
    if (!xformat.empty()) {
      //TODO
      h -= number_margin;
    }
    
    //Compute ynumbers
    if (!yformat.empty()) {
      //TODO
      x += number_margin;
      w -= number_margin;
    }
  }

  //Graph area
  ByteImage graph(h, w, 3);
  memset(graph.pixels, 0xFF, graph.size());

  //Draw axes
  DrawRect(graph, axis_offset, 0, 
	   axis_thickness, graph.nr - axis_offset, 0);
  DrawRect(graph, axis_offset, graph.nr - axis_offset - axis_thickness, 
	   graph.nc - axis_offset - 1, axis_thickness, 0);
  
  //Draw ticks
  std::vector<int> xticks, yticks;
  std::vector<double> xtickvalues, ytickvalues;
  //TODO

  //Draw numbers
  //TODO
  
  //Draw plots
  graph = graph.toColor();
  //TODO

  //Blit graph to rest of image
  img.blit(graph, y, x);

  return img;
}
