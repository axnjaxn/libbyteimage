#ifndef _BYTEIMAGE_NO_SDL2
#include <byteimage/byteimage_sdl2.h>
#endif
#include <byteimage/plotter.h>

int main(int argc, char* argv[]) {
  Plotter plotter;

  Plotter::Plot plot;
  int n = 100;
  for (int x = 0; x < n; x++)
    plot.addPoint((double)x / (n - 1), 0.001 * (rand() % 1000));
  plotter.addPlot(plot);

  plotter.setTitleFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 12);
  plotter.setTitle("Title Goes Here");

  plotter.setLabelFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 10);
  plotter.setXLabel("X Unit Label");
  plotter.setYLabel("Y Unit Label");

  plotter.setNumberFont("/usr/share/fonts/truetype/freefont/FreeSerif.ttf", 8);
  plotter.setNumberFormat("%.1lf", "");

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
