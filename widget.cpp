#include "widget.h"

using namespace byteimage;

bool WidgetLayout::Attach::overlaps(int x, int y) const {
  return (x >= this->x && x < this->x + this->w
	  && y >= this->y && y < this->y + this->h);
}

WidgetLayout::~WidgetLayout() {
  clear();  
}

void WidgetLayout::attach(Widget* widget, int x, int y, int w, int h, bool autorelease) {
  Attach attach(widget, x, y, w, h, autorelease);
  widget->setDisplay(display);
  widget->updateSize(w, h);
  widgets.push_back(attach);
}

void WidgetLayout::clear() {
  for (auto& attach : widgets)
    if (attach.autorelease)
      delete attach.widget;
  widgets.clear();
  focus = -1;
}

void WidgetLayout::setDisplay(WidgetDisplay* display) {
  Widget::setDisplay(display);
  for (auto& attach : widgets)
    attach.widget->setDisplay(display);
}

void WidgetLayout::handleEvent(SDL_Event event) {
  SDL_Event transformed;
  if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
    for (int i = widgets.size() - 1; i >= 0; i--)
      if (widgets[i].overlaps(event.button.x, event.button.y)) {
	focus = i;
	transformed = event;
	transformed.button.x -= widgets[i].x;
	transformed.button.y -= widgets[i].y;
	widgets[i].widget->handleEvent(transformed);
	return;
      }
    focus = -1;
  }
  else if (event.type == SDL_MOUSEMOTION && focus >= 0) {
    if (widgets[focus].overlaps(event.motion.x, event.motion.y)) {
      transformed = event;
      transformed.motion.x -= widgets[focus].x;
      transformed.motion.y -= widgets[focus].y;
      widgets[focus].widget->handleEvent(transformed);
    }
  }
}

void WidgetLayout::render(ByteImage& canvas, int x, int y) {
  for (auto& attach : widgets)
    attach.widget->render(canvas, x + attach.x, y + attach.y);
}

void WidgetDisplay::handleEvent(SDL_Event event) {
  if (event.type == SDL_MOUSEBUTTONDOWN
      || event.type == SDL_MOUSEBUTTONUP
      || event.type == SDL_MOUSEMOTION)
    layout.handleEvent(event);

  Display::handleEvent(event);
}

void WidgetDisplay::render() {
  canvas.fill(0);
  layout.render(canvas, 0, 0);
}

void WidgetDisplay::update() {
  if (renderflag) {
    render();
    updateImage(canvas);
    renderflag = false;    
  }
  
  Display::update();
}

WidgetDisplay::WidgetDisplay(int nr, int nc, const char* title) : Display(nr, nc, title) {
  canvas = ByteImage(nr, nc, 3);
  layout.setDisplay(this);
  layout.updateSize(nc, nr);
  renderflag = 1;
}
