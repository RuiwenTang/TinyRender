#include <Color.h>

namespace TRM {

Color::Color() : fRGBA{0, 0, 0, 0} {}
Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : fRGBA{r, g, b, a} {}

Color Color::blend(const Color& src) const {
  float alpha = float(src.fRGBA[color_A]) / 255.f;
  float one_minus_alpha = 1.f - alpha;
  
  uint8_t r = src.fRGBA[color_R] + uint8_t(this->fRGBA[color_R] * one_minus_alpha);
  uint8_t g = src.fRGBA[color_G] + uint8_t(this->fRGBA[color_G] * one_minus_alpha);
  uint8_t b = src.fRGBA[color_B] + uint8_t(this->fRGBA[color_G] * one_minus_alpha);
  uint8_t a = src.fRGBA[color_A] + uint8_t(this->fRGBA[color_A] * one_minus_alpha);

  return Color(r, g, b, a);
}

Color Color::ColorTransparent() { return Color(); }

Color Color::ColorBlack() { return Color(0, 0, 0, 0xFF); }

Color Color::ColorDarkGray() { return Color(0x44, 0x44, 0x44, 0xFF); }

Color Color::ColorGray() { return Color(0x88, 0x88, 0x88, 0xFF); }

Color Color::ColorLightGray() { return Color(0xCC, 0xCC, 0xCC, 0xFF); }

Color Color::ColorWhite() { return Color(0xFF, 0xFF, 0xFF, 0xFF); }

Color Color::ColorRed() { return Color(0xFF, 0x00, 0x00, 0xFF); }

Color Color::ColorGreen() { return Color(0x00, 0xFF, 0x00, 0xFF); }

Color Color::ColorBlue() { return Color(0x00, 0x00, 0xFF, 0xFF); }

Color Color::ColorYellow() { return Color(0xFF, 0xFF, 0x00, 0xFF); }

Color Color::ColorCyan() { return Color(0x00, 0xFF, 0xFF, 0xFF); }

Color Color::ColorMagenta() { return Color(0xFF, 0x00, 0xFF, 0xFF); }

}  // namespace TRM