#include <byteimage/spline.h>
#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>

class SplineDisplay : public ByteImageDisplay {
protected:
  ByteImage canvas;
  Spline spline;
  int nsteps = 200;
  int sel = -1;
  double sel_r = 3.0;

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
      Matrix m {(double)event.button.x, (double)event.button.y};
      for (int i = 0; i < spline.countPoints(); i++)
	if (sqLength(spline[i] - m) <= sel_r * sel_r) sel = i;      
    }
    else if (event.type == SDL_MOUSEMOTION) {
      if (sel >= 0) {
	Matrix m {(double)event.button.x, (double)event.button.y};
	spline[sel] = m;
	drawflag = 1;
      }
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
      sel = -1;
    }
    else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_UP:
	nsteps += 40;
	drawflag = 1;
	break;
      case SDLK_DOWN:
	nsteps -= 40;
	drawflag = 1;
	break;
      }
    }
    ByteImageDisplay::handleEvent(event);
  }
  void render() {
    if (!drawflag) return;
    canvas.fill(0);
    for (int i = 0; i < spline.countPoints(); i++)
      DrawPoint(canvas,
		{spline[i].at(0), spline[i].at(1), 1}, 
		{192, 192, 192}, 5);

    for (int i = 1; i < spline.countPoints(); i++)
      DrawLine(canvas,
	       {spline[i - 1].at(0), spline[i - 1].at(1), 1},
	       {spline[i].at(0), spline[i].at(1), 1},
	       {192, 192, 192}, 1);
    
    double scale = (spline.domainEnd() - spline.domainStart()) / (nsteps - 1);
    Matrix x0, x1 = spline.at(spline.domainStart());
    for (int i = 0; i < nsteps; i++) {
      x0 = x1;
      x1 = spline.at(spline.domainStart() + i * scale);
      DrawLine(canvas, {x0.at(0), x0.at(1), 1}, {x1.at(0), x1.at(1), 1}, {255.0, 255.0, 255.0});
    }
    
    for (int i = spline.degree() - 1; i < spline.countKnots() - spline.degree() + 1; i++) {
      x0 = spline.at(spline.getKnot(i));
      DrawPoint(canvas,
		{x0.at(0), x0.at(1), 1},
		{255, 255, 255}, 3);
    }

    updateImage(canvas);
  }
  void update() {
    render();
    ByteImageDisplay::update();
  }

public:
  SplineDisplay() : ByteImageDisplay(512, 512), canvas(512, 512, 3) {
    spline = Spline::makeSpline(3, {
	{20, 256}, {100, 256}, {180, 256}, {260, 192}, {340, 256}, {420, 256}, {500, 256}
      });
  }
};

int main(int argc, char* argv[]) {
  SplineDisplay().main();
  return 0;
}
