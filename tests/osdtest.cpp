#include <byteimage/osd.h>

using namespace byteimage;

class MyDisplay : public Display {
protected:
  OSD_Printer osd;
  OSD_Scanner scanner;
  ByteImage img, canvas;
  
  void printHelp() {
    osd.print("Press i for ints, f for floats, d for double, s for string, h for help.", 10000);
  }
  
  virtual void handleEvent(SDL_Event event) {
    int i;
    float f;
    double d;
    std::string s;
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
      case SDLK_i:
	osd.hide();
	if (scanner.getInt(canvas, "Enter an int:", i)) 
	  osd.print(OSD_Printer::string("Test: %d", i), 2000);
	break;
      case SDLK_f:
	osd.hide();
	if (scanner.getFloat(canvas, "Enter a float:", f)) 
	  osd.print(OSD_Printer::string("Test: %f", f), 2000);
	break;
      case SDLK_d:
	osd.hide();
	if (scanner.getDouble(canvas, "Enter a double:", d)) 
	  osd.print(OSD_Printer::string("Test: %lf", d), 2000);
	break;
      case SDLK_s:
	osd.hide();
	if (scanner.getString(canvas, "Enter a string:", s)) 
	  osd.print(OSD_Printer::string("Test: %s", s.c_str()), 2000);
	break;
      case SDLK_h:
	printHelp();
	break;
      }
    }
    Display::handleEvent(event);
  }

  virtual void update() {
    if (osd.shouldDraw()) {
      canvas = img;
      osd.draw(canvas);
      updateImage(canvas);
    }
    
    Display::update();
  }
  
public:
  MyDisplay() : Display(600, 800) {
    img = ByteImage(600, 800);

    for (int i = 0; i < img.size(); i++)
      if (((i % 599) / 5) % 2) img[i] = 64;

    canvas = img = img.toColor();
    updateImage(img);

    osd.setColors(Color(255, 255, 0), Color(0));
    scanner.setColors(Color(0), Color(255));
    scanner.setDisplay(this);
    
    printHelp();
  }
};

int main(int argc, char* argv[]) {
  MyDisplay display;
  TextRenderer font("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 22);
  OSD_Scanner::setFont(&font);
  OSD_Printer::setFont(&font);
  display.main();
  return 0;
}
