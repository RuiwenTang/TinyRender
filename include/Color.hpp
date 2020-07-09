//
// Created by TangRuiwen on 2020/7/9.
//

#ifndef TINYRENDER_INCLUDE_COLOR_HPP_
#define TINYRENDER_INCLUDE_COLOR_HPP_

namespace TRM {

struct Color {
  unsigned char bgra[4];
  unsigned char bytespp;

  Color() : bgra(), bytespp(1) {
    for (int i = 0; i < 4; i++) bgra[i] = 0;
  }

  Color(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : bgra(), bytespp(4) {
    bgra[0] = B;
    bgra[1] = G;
    bgra[2] = R;
    bgra[3] = A;
  }

  Color(unsigned char v) : bgra(), bytespp(1) {
    for (int i = 0; i < 4; i++) bgra[i] = 0;
    bgra[0] = v;
  }

  Color(const unsigned char *p, unsigned char bpp) : bgra(), bytespp(bpp) {
    for (int i = 0; i < (int) bpp; i++) {
      bgra[i] = p[i];
    }
    for (int i = bpp; i < 4; i++) {
      bgra[i] = 0;
    }
  }

  unsigned char &operator[](const int i) { return bgra[i]; }

  Color operator*(float intensity) const {
    Color res = *this;
    intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
    for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * intensity;
    return res;
  }

  static Color TRANSPARENT;
  static Color BLACK;
  static Color GRAY;
  static Color WHITE;
  static Color RED;
  static Color GREEN;
  static Color BLUE;
  static Color YELLOW;
  static Color CYAN;
  static Color MAGENTA;
};



}

#endif //TINYRENDER_INCLUDE_COLOR_HPP_
