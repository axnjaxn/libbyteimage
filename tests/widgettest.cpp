#include <byteimage/widget.h>
#include <byteimage/render.h>

using namespace byteimage;

class RGBDisplayWidget : public Widget {
protected:
  Color* color;

public:
  RGBDisplayWidget(Color* color) : Widget() {this->color = color;}
  
  void render(ByteImage& canvas, int x, int y) {
    DrawRect(canvas, x, y, w, h, color->r, color->g, color->b);
  }
};

class RGBWidget : public Widget {
protected:
  Color* color;

public:
  RGBWidget(Color* color) : Widget() {this->color = color;}

  void set(int ch, int x) {
    switch (ch) {
    default:
    case 0: color->r = clip(x); break;
    case 1: color->g = clip(x); break;
    case 2: color->b = clip(x); break;
    }
    display->setRenderFlag();
  }

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
      set(3 * event.button.y / h, event.button.x * 256 / w);
    else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
      set(3 * event.motion.y / h, event.motion.x * 256 / w);
  }

  void render(ByteImage& canvas, int x, int y) {
    int xsc = w / 256;
    int ysc = h / 3;
    int col[] = {color->r, color->g, color->b};
    for (int ch = 0; ch < 3; ch++) {
      for (int i = 0; i < 256; i++)
	if (col[ch] == i)
	  DrawRect(canvas, x + i * xsc, y + ch * ysc, xsc, ysc, 255, 255, 255);
	else 
	  DrawRect(canvas, x + i * xsc, y + ch * ysc, xsc, ysc, (ch == 0)? i : 0, (ch == 1)? i : 0, (ch == 2)? i : 0);
    }
  }
};

class HueWidget : public Widget {
protected:
  Color* color;

public:
  HueWidget(Color* color) : Widget() {this->color = color;}
  
  void set(float x) {
    float h, s, l;
    rgb2hsl(color->r, color->g, color->b, h, s, l);
    hsl2rgb(x, s, l, color->r, color->g, color->b);
    display->setRenderFlag();
  }

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
      set(360.0 * event.button.x / w);
    else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
      set(360.0 * event.motion.x / w);
  }

  void render(ByteImage& canvas, int x, int y) {
    float h;
    Byte r, g, b;

    for (int i = 0; i < w; i++) {
      h = 360.0 * i / w;
      hsl2rgb(h, 1.0, 0.5, r, g, b);
      DrawRect(canvas, x + i, y, 1, this->h, 
	       r, g, b);
    }

    float dummy, dummy1;
    rgb2hsl(color->r, color->g, color->b, h, dummy, dummy1);
    DrawRect(canvas, x + (int)(h / 360.0 * w), y, 1, this->h, 255, 255, 255);
  }
};

class SatWidget : public Widget {
  protected:
  Color* color;

public:
  SatWidget(Color* color) : Widget() {this->color = color;}
  
  void set(float x) {
    float h, s, l;
    rgb2hsl(color->r, color->g, color->b, h, s, l);
    hsl2rgb(h, x, l, color->r, color->g, color->b);
    display->setRenderFlag();
  }

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
      set((float)event.button.x / w);
    else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
      set((float)event.motion.x / w);
  }

  void render(ByteImage& canvas, int x, int y) {
    float h, s, S, l;
    Byte r, g, b;
    rgb2hsl(color->r, color->g, color->b, h, S, l);

    for (int i = 0; i < w; i++) {
      s = (float)i / w;
      hsl2rgb(h, s, l, r, g, b);
      DrawRect(canvas, x + i, y, 1, this->h, r, g, b);
    }
    
    DrawRect(canvas, x + (int)(S * w), y, 1, this->h, 255, 255, 255);
  }
};

class LumWidget : public Widget {
  protected:
  Color* color;

public:
  LumWidget(Color* color) : Widget() {this->color = color;}
  
  void set(float x) {
    float h, s, l;
    rgb2hsl(color->r, color->g, color->b, h, s, l);
    l = (double)x;
    hsl2rgb(h, s, x, color->r, color->g, color->b);
    display->setRenderFlag();
  }

  void handleEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
      set((float)event.button.x / w);
    else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK))
      set((float)event.motion.x / w);
  }

  void render(ByteImage& canvas, int x, int y) {
    float h, s, l, L;
    Byte r, g, b;
    rgb2hsl(color->r, color->g, color->b, h, s, L);

    for (int i = 0; i < w; i++) {
      l = (float)i / w;
      hsl2rgb(h, s, l, r, g, b);
      DrawRect(canvas, x + i, y, 1, this->h, r, g, b);
    }
    
    DrawRect(canvas, x + (int)(L * w), y, 1, this->h, 255, 255, 255);
  }
};

class ColorPicker : public WidgetDisplay {
protected:
  Color color;

  void handleEvent(SDL_Event event) {
    float h, s, l;
    int ri, gi, bi;
    if (event.type == SDL_KEYDOWN)
      switch (event.key.keysym.sym) {
      case SDLK_0:
	color = Color();
	setRenderFlag();
	break;
      case SDLK_h:
	printf("Enter h, s, l:\n");
	scanf("%f %f %f", &h, &s, &l);
	hsl2rgb(h, s, l, color.r, color.g, color.b);
	setRenderFlag();
	break;
      case SDLK_r:
	printf("Enter r, g, b:\n");
	scanf("%d %d %d", &ri, &gi, &bi);
	color.r = ri;
	color.g = gi;
	color.b = bi;
	setRenderFlag();
	break;
      }
    WidgetDisplay::handleEvent(event);
  }

  void render() {
    char title[256];
    sprintf(title, "Color Picker: #%02x%02x%02x", color.r, color.g, color.b);
    SDL_SetWindowTitle(window, title);
    WidgetDisplay::render();
  }
  
public:
  ColorPicker() : WidgetDisplay(256, 512, "Color Picker") {
    int y = 0;
    layout.attach(new RGBDisplayWidget(&color), 0, y, 512, 64);
    layout.attach(new RGBWidget(&color), 0, y += 64, 512, 96);
    layout.attach(new HueWidget(&color), 0, y += 96, 512, 32);
    layout.attach(new SatWidget(&color), 0, y += 32, 512, 32);
    layout.attach(new LumWidget(&color), 0, y += 32, 512, 32);
  }
};

int main(int argc, char* argv[]) {
  ColorPicker().main();
  return 0;
}
