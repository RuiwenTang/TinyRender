#include <Color.h>

namespace TRM {

Color::Color() : fRGBA{0, 0, 0, 0} {}
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : fRGBA{r, g, b, a} {}

Color Color::ColorTransparent() { return Color(); }

Color Color::ColorBlack() { return Color(0, 0, 0, 0xFF); }

Color Color::ColorDarkGray() { return Color(0x44, 0x44, 0x44, 0xFF); }

Color Color::ColorGray() { return Color(0x88, 0x88, 0x88, 0xFF); }

Color Color::ColorLightGray() { return Color(0xCC, 0xCC, 0xCC, 0xFF); }

Color Color::ColorWhite() { return Color(0xFF, 0xFF, 0xFF, 0xFF); }

Color Color::ColorRed() { return Color(0xFF, 0x00, 0x00, 0xFF); }

Color Color::ColorBlue() { return Color(0x00, 0xFF, 0x00, 0xFF); }

Color Color::ColorYellow() { return Color(0xFF, 0xFF, 0x00, 0xFF); }

Color Color::ColorCyan() { return Color(0x00, 0xFF, 0xFF, 0xFF); }

Color Color::ColorMagenta() { return Color(0xFF, 0x00, 0xFF, 0xFF); }

}  // namespace TRM