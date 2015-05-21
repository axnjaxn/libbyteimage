#include <byteimage/byteimage_sdl2.h>
#include <byteimage/render.h>
#include <vector>

using namespace byteimage;

class ListeningDisplay : public Display {
public:
  typedef void (*EventListener)(ListeningDisplay& display, SDL_Event event, void* data);
  typedef void (*FrameListener)(ListeningDisplay& display, void* data);

protected:
  typedef struct {
    EventListener listener;
    void* data;
  } ConnectedEvent;
  std::vector<ConnectedEvent> event_listeners;
  
  typedef struct {
    FrameListener listener;
    void* data;
  } ConnectedFrame;
  std::vector<ConnectedFrame> frame_listeners;  

  virtual void handleEvent(SDL_Event event) {
    for (auto L: event_listeners)
      L.listener(*this, event, L.data);
    Display::handleEvent(event);
  }

  virtual void update() {
    for (auto L : frame_listeners)
      L.listener(*this, L.data);
    Display::update();
  }
  
public:
  ListeningDisplay(int nr, int nc, const char* title = nullptr) : Display(nr, nc, title) { }
  ListeningDisplay(const ByteImage& img, const char* title = nullptr) : Display(img, title) { }

  void addEventListener(EventListener listener, void* data) {event_listeners.push_back({listener, data});}
  void removeEventListener(EventListener listener) {
    for (int i = 0; i < event_listeners.size(); i++)
      if (event_listeners[i].listener == listener)
	event_listeners.erase(event_listeners.begin() + i--);
  }
  
  void addFrameListener(FrameListener listener, void* data) {frame_listeners.push_back({listener, data});}
  void removeFrameListener(FrameListener listener) {
    for (int i = 0; i < frame_listeners.size(); i++)
      if (frame_listeners[i].listener == listener)
	frame_listeners.erase(frame_listeners.begin() + i--);
  }

  virtual void updateImage(const ByteImage& img) {Display::updateImage(img);}
};

float G(float x, float sigma) {
  const float pi = 3.14159265358979;
  float s2 = sigma * sigma;
  float s2inv = 1.0 / s2;
  float fact = pow(2 * pi * s2, -0.5);
  return fact * exp(-0.5 * x * x * s2inv);
}

class DisplayData {
public:
  ByteImage canvas = ByteImage(256, 256, 3);
  bool renderflag = true;
};

void keyEvent(ListeningDisplay& display, SDL_Event event, void* v) {
  DisplayData& data = *(DisplayData*)v;
  if (event.type == SDL_KEYDOWN)
    switch (event.key.keysym.sym) {
    case SDLK_RETURN:
      data.renderflag = true;
      break;
    case SDLK_SPACE:
      data.canvas.fill(0);
      data.renderflag = true;
      break;
    }
}

void render(ListeningDisplay& display, void* v) {
  DisplayData& data = *(DisplayData*)v;
  if (data.renderflag) {
    const int N = 5;
    Pt2f center, v;
    float radius;
    for (int i = 0; i < N; i++) {
      center.x = (rand() % 256);
      center.y = (rand() % 256);
      v = center / 128.0 - Pt2f(1.0, 1.0);
      radius = 60.0 * G(v.length(), 0.55);
      DrawCircle(data.canvas, center, Color(255), radius);
    }
    display.updateImage(data.canvas);
    data.renderflag = false;
  }
}

int main(int argc, char* argv[]) {
  DisplayData data;
  ListeningDisplay display(data.canvas);
  display.addEventListener(keyEvent, &data);
  display.addFrameListener(render, &data);
  display.main();
  
  return 0;
}
