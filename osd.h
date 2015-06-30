#include "byteimage_sdl2.h"
#include "font.h"

namespace byteimage {
  class OSD_Scanner {
  protected:
    static TextRenderer* font;
  
    Display* display = nullptr;
    Color color = Color(255), bgcolor;
    bool draw_bg = false;

  public:
    //Only one global font is allowed with this class
    static void setFont(TextRenderer* font);
    
    void setDisplay(Display* display);
    void setColors(Color color);
    void setColors(Color color, Color bgcolor);

    //These functions return true if a valid input is captured
    bool getInt(const ByteImage& target, const std::string& prompt, int& v);
    bool getFloat(const ByteImage& target, const std::string& prompt, float& v);
    bool getDouble(const ByteImage& target, const std::string& prompt, double& v);  
    bool getString(const ByteImage& target, const std::string& prompt, std::string& v);
  };

  class OSD_Printer {
  protected:
    static TextRenderer* font; 
    bool isPrinting = false, isHidden = false;
    Uint32 print_end;
    std::string print_buf;
  
    Color color = Color(255), bgcolor;
    bool draw_bg = false;

  public:
    static void setFont(TextRenderer* font);
    static std::string string(const char* str, ...);

    void setColors(Color color);
    void setColors(Color color, Color bgcolor);
    
    void print(const std::string& str, int ms = 3000);
    bool shouldDraw() const;//True if visible, or on first frame after turning invisible

    void hide();
    void draw(ByteImage& target);//Note: if no printing required, this is a no-op.
  };
}
