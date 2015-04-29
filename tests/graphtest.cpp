#ifndef _BYTEIMAGE_NO_SDL2
#include <byteimage/byteimage_sdl2.h>
#endif
#include <byteimage/plotter.h>

using namespace byteimage;

int main(int argc, char* argv[]) {
  Plotter plotter;

  Plotter::Plot plot;
  int n = 30;
  for (int x = 0; x < n; x++)
    plot.addPoint((double)x / (n - 1), 0.001 * (rand() % 1000));
  plot.setLineSize(1);
  plot.setColor(255, 0, 0);
  plotter.addPlot(plot);

  plot = Plotter::Plot();
  n = 300;
  for (int x = 0; x < n; x++)
    plot.addPoint((double)x / (n - 1), 0.5 * cos(40.0 * x / (n - 1)) + 0.5);
  plot.setPointSize(0);
  plot.setColor(0, 0, 255);
  plot.setLineSize(1);
  plotter.addPlot(plot);

  plotter.setTitleFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 22);
  plotter.setTitle("Title Goes Here");

  plotter.setLabelFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 18);
  plotter.setXLabel("X Unit Label");
  plotter.setYLabel("Y Unit Label");

  plotter.setNumberFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 12);
  plotter.setNumberFormat("%.1lf", "%.2lf");

  ByteImage graph = plotter.render(480, 640);

#ifndef _BYTEIMAGE_NO_SDL2
  display(graph);
#else
  const char* fn = "graph.png";
  graph.save_filename(fn);
  printf("Graph saved to %s.\n", fn);
#endif

  return 0;
}
