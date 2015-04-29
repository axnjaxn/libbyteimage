#include "plotter.h"
#include "render.h"

using namespace byteimage;

Plotter::Plot::Plot() {
  setPointSize(3);
  setLineSize(0);
  setColor(0, 0, 0);
}

void Plotter::Plot::addPoint(double x, double y) {
  points.push_back(makePoint(x, y));
}

void Plotter::Plot::setColor(double r, double g, double b) {
  color = makeColor(r, g, b);
}

void Plotter::Plot::setPointSize(int size) {
  has_bar = 0;
  point_size = size;
}

void Plotter::Plot::setLineSize(int size) {
  has_bar = 0;
  line_size = size;
}

void Plotter::Plot::setBar(bool enabled) {
  has_bar = enabled;
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

static int getDefaultMargin(int size) {
  return (int)(size / 72.0 * TextRenderer::dpi + 5);
}

void Plotter::setTitleFont(const std::string& name, int size, int margin) {
  if (titleRender) delete titleRender;
  titleRender = new TextRenderer(name.c_str(), size);
  title_margin = margin? margin : getDefaultMargin(size);
}

void Plotter::setTitle(const std::string& title) {
  this->title = title;
}

void Plotter::setLabelFont(const std::string& name, int size, int margin) {
  if (labelRender) delete labelRender;
  labelRender = new TextRenderer(name.c_str(), size);
  label_margin = margin? margin : getDefaultMargin(size);
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
  number_margin = margin? margin : getDefaultMargin(size);
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
  if (titleRender && title_margin && !title.empty()) {
    titleRender->drawCentered(img, title.c_str(), y + h - title_margin / 2, x + w / 2, 0);
    h -= title_margin;
  }
  
  if (labelRender && label_margin) {
    //Compute xlabel
    if (!xlabel.empty()) {
      //TODO: Don't leave space if the ylabel isn't drawn
      ByteImage textBox(label_margin, w - label_margin);
      memset(textBox.pixels, 0xFF, textBox.size());
      labelRender->drawCentered(textBox, xlabel.c_str(),
			       textBox.nr / 2, textBox.nc / 2, 0);
      img.blit(textBox, y + h - label_margin, x + label_margin);
      
      h -= label_margin;
    }

    //Compute ylabel
    if (!ylabel.empty()) {
      ByteImage textBox(label_margin, h);
      memset(textBox.pixels, 0xFF, textBox.size());
      labelRender->drawCentered(textBox, ylabel.c_str(),
			       textBox.nr / 2, textBox.nc / 2, 0);
      img.blit(textBox.rotatedCCW(), y, x);

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
  
  //Compute value-to-graph matrix
  Matrix T = Matrix::identity(3);
  double a = axis_offset + axis_thickness * 0.5;
  double f = (w - a) / (mx.at(0) - mn.at(0));
  T.at(0, 0) = f;
  T.at(0, 2) = a - f * mn.at(0);
  a = h - a;
  f = -a / (mx.at(1) - mn.at(1));
  T.at(1, 1) = f;
  T.at(1, 2) = a - f * mn.at(1);

  //Draw ticks
  std::vector<int> xticks, yticks;
  std::vector<double> xtickvalues, ytickvalues;
  //TODO

  //Draw grid
  //TODO

  //Draw numbers
  //TODO
  
  //Draw plots
  graph = graph.toColor();
  for (int i = 0; i < plots.size(); i++) {
    if (plots[i].line_size)
      for (int j = 1; j < plots[i].points.size(); j++)
	DrawLine(graph, T * plots[i].points[j - 1], T * plots[i].points[j], plots[i].color, plots[i].line_size);

    if (plots[i].point_size)
      for (int j = 0; j < plots[i].points.size(); j++)
	DrawPoint(graph, T * plots[i].points[j], plots[i].color, plots[i].point_size);
  }
  //TODO

  //Blit graph to rest of image
  img.blit(graph, y, x);

  return img;
}
