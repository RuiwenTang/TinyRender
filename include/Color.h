#pragma once

#include <array>
#include <cstdint>

namespace TRM {

/**
 * @brief Simple wrapper for color, only support RGBA color
 *
 */
class Color final {
  enum {
    color_R = 0,
    color_G = 1,
    color_B = 2,
    color_A = 3,
  };

 public:
  Color();
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
  ~Color() = default;

  std::array<uint8_t, 4>& rgba() { return fRGBA; }

  Color(Color const&) = default;
  Color& operator=(Color const&) = default;

  static Color ColorTransparent();
  static Color ColorBlack();
  static Color ColorDarkGray();
  static Color ColorGray();
  static Color ColorLightGray();
  static Color ColorWhite();
  static Color ColorRed();
  static Color ColorGreen();
  static Color ColorBlue();
  static Color ColorYellow();
  static Color ColorCyan();
  static Color ColorMagenta();

 private:
  std::array<uint8_t, 4> fRGBA;
};

}  // namespace TRM