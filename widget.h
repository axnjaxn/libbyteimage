#ifndef _BPJ_BYTEIMAGE_WIDGET_H
#define _BPJ_BYTEIMAGE_WIDGET_H

#include <byteimage/byteimage_sdl2.h>
#include <vector>

using byteimage::ByteImage;
using byteimage::Display;

class WidgetDisplay;//fwd. decl.

/*
 * Subclassing widget:
 * 1. Override handleEvent for mouse interaction (optional)
 * 2. Override render, keeping in mind w and h
 * 3. When instantiating, make sure display is initialized somewhere
 */

class Widget {
protected:
  WidgetDisplay* display;
  int w, h;

public:
  inline Widget(WidgetDisplay* display = nullptr) {w = h = 0; setDisplay(display);}

  inline virtual void setDisplay(WidgetDisplay* display) {this->display = display;}
  inline virtual void updateSize(int w, int h) {this->w = w; this->h = h;}

  inline int width() const {return w;}
  inline int height() const {return h;}
  
  virtual void handleEvent(SDL_Event event) { }//Only mouse down and mouse up by default, motion if focused
  virtual void render(ByteImage& canvas, int x, int y) = 0;
};

/*
 * Using layouts:
 * 1. Attachment will automatically provide display pointer if possible
 * 2. Do not copy any layouts when autorelease is enabled
 * 3. Only focused (clicked) widgets get motion events
 * 
 * Everything will be handled hierarchically as necessary.
 */

class WidgetLayout : public Widget {
public:
  class Attach {
  public:
    Widget* widget;
    int x, y, w, h;
    bool autorelease;

    inline Attach() : Attach(nullptr, 0, 0, 0, 0) { }
    inline Attach(Widget* widget, int x, int y, int w, int h, bool autorelease = true)
      : widget(widget), x(x), y(y), w(w), h(h), autorelease(autorelease) { }
    
    bool overlaps(int x, int y) const;
  };

  int focus = -1;
  std::vector<Attach> widgets;
  
  WidgetLayout(WidgetDisplay* display = nullptr) : Widget(display) { }
  virtual ~WidgetLayout();

  void attach(Widget* widget, int x, int y, int w, int h, bool autorelease = true);
  void clear();
  
  virtual void setDisplay(WidgetDisplay* display);
  virtual void handleEvent(SDL_Event event);
  virtual void render(ByteImage& canvas, int x, int y);
};

class WidgetDisplay : public Display {
protected:
  WidgetLayout layout;
  ByteImage canvas;
  bool renderflag;

  virtual void handleEvent(SDL_Event event);
  virtual void render();
  virtual void update();

public:
  WidgetDisplay(int nr, int nc, const char* title = "WidgetDisplay");

  inline void setRenderFlag() {renderflag = true;}
};

#endif
