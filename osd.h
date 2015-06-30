#include "byteimage_sdl2.h"
#include "font.h"

namespace byteimage {
  class OSD_Scanner {
  public:
    static TextRenderer* font;
  
    Display* display;
    ByteImage bg, canvas;
    Color color;

    //Normally, you'll instantiate this class each time you want to scan for input.
    //Note: this will require the program to have a static background while scanning.
    OSD_Scanner(Display& display, const ByteImage& bg, Color color = Color(255));

    //Only one global font is allowed with this class
    static void setFont(TextRenderer* font);

    //These functions return true if a valid input is captured
    bool getInt(const std::string& prompt, int& v);
    bool getFloat(const std::string& prompt, float& v);
    bool getDouble(const std::string& prompt, double& v);  
    bool getString(const std::string& prompt, std::string& v);
  };

  class OSD_Printer {
  protected:
    static TextRenderer* font; 
    bool isPrinting = false, isHidden = false;
    Uint32 print_end;
    std::string print_buf;
  
  public:
    static void setFont(TextRenderer* font);
    static std::string string(const char* str, ...);
  
    void print(const std::string& str, int ms = 3000);
    bool shouldDraw() const;//True if visible, or on first frame after turning invisible

    void hide();
    void draw(ByteImage& target);//Note: if no printing required, this is a no-op.
  };
}
