#include "osd.h"

using namespace byteimage;

TextRenderer* OSD_Scanner::font = nullptr;
TextRenderer* OSD_Printer::font = nullptr;

OSD_Scanner::OSD_Scanner(Display& display, const ByteImage& bg, Color color) : display(&display), bg(bg), color(color) { }

void OSD_Scanner::setFont(TextRenderer* font) {OSD_Scanner::font = font;}
  
bool OSD_Scanner::getInt(const std::string& prompt, int& v) {
  std::string str;
  if (!getString(prompt, str)) return false;
  return (sscanf(str.c_str(), "%d", &v) == 1);
}
  
bool OSD_Scanner::getFloat(const std::string& prompt, float& v) {
  std::string str;
  if (!getString(prompt, str)) return false;
  return (sscanf(str.c_str(), "%f", &v) == 1);
}
  
bool OSD_Scanner::getDouble(const std::string& prompt, double& v) {
  std::string str;
  if (!getString(prompt, str)) return false;
  return (sscanf(str.c_str(), "%lf", &v) == 1);
}

bool OSD_Scanner::getString(const std::string& prompt, std::string& v) {
  bool drawflag = true, doneflag = false;
  std::string scan_buf;
  std::vector<SDL_Event> captured_events;
  SDL_Event event;

  if (display->show(bg)) return false;
    
  SDL_StartTextInput();
  while (!doneflag) {      
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) {
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	  for (SDL_Event& event : captured_events)
	    SDL_PushEvent(&event);
	  display->show(bg);
	  SDL_StopTextInput();
	  return false;
	case SDLK_RETURN:
	  doneflag = true;
	  break;
	case SDLK_BACKSPACE:
	  if (scan_buf.size() > 0) {
	    scan_buf = scan_buf.substr(0, scan_buf.size() - 1);
	    drawflag = true;
	  }
	  break;
	}
      }
	
      if (event.type == SDL_TEXTINPUT) {
	scan_buf = scan_buf + std::string(event.text.text);
	drawflag = true;
      }

      captured_events.push_back(event);
    }

    for (SDL_Event& event : captured_events)
      SDL_PushEvent(&event);
    captured_events.clear();

    if (drawflag) {
      int x, y, w, h;
	
      std::string print_str = prompt + " " + scan_buf;
      font->getBox(print_str.c_str(), x, y, w, h);
	
      canvas = bg;
      font->draw(canvas, print_str.c_str(), canvas.nr - h - y - 1, -x, color.r, color.g, color.b);

      drawflag = false;
    }

    if (display->show(canvas)) {
      SDL_StopTextInput();
      return false;
    }
  }
  SDL_StopTextInput();

  display->show(bg);
  v = scan_buf;
  return true;
}

void OSD_Printer::setFont(TextRenderer* font) {OSD_Printer::font = font;}

std::string OSD_Printer::string(const char* str, ...) {
  va_list args;
  va_start(args, str);
  char buf[1024];
  vsprintf(buf, str, args);
  return std::string(buf);
}

void OSD_Printer::print(const std::string& str, int ms) {
  isPrinting = true;
  print_buf = str;
  print_end = SDL_GetTicks() + ms;
}

bool OSD_Printer::shouldDraw() const {
  return (isPrinting || isHidden);
}

void OSD_Printer::hide() {
  print_end = 0;
}

void OSD_Printer::draw(ByteImage& target) {
  isHidden = isPrinting && SDL_GetTicks() > print_end;
  isPrinting = isPrinting && !isHidden;
  if (isPrinting) {
    int x, y, w, h;
    font->getBox(print_buf.c_str(), x, y, w, h);
    font->draw(target, print_buf.c_str(), -y, -x, 255);
  }
}
