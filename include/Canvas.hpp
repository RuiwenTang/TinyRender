//
// Created by TangRuiwen on 2020/7/9.
//

#ifndef TINYRENDER_INCLUDE_CANVAS_HPP_
#define TINYRENDER_INCLUDE_CANVAS_HPP_

#include "Geometry.h"
#include "Color.hpp"
#include <memory>

namespace TRM {

class TGAImage;

class Canvas {
 public:
  virtual ~Canvas() = default;
  static std::unique_ptr<Canvas> MakeRasterDirect(TGAImage *image);
  void clear(const Color &color) { this->onClearColor(color); }
  void drawLine(int x0, int y0, int x1, int y1, uint32_t lineWidth, const Color &color);
  void drawCircle(int centerX, int centerY, int radius, const Color& color);

 protected:
  virtual void onDrawTriangle(const Vec2i &a,
                              const Vec2i &b,
                              const Vec2i &c,
                              const Color &colorA,
                              const Color &colorB,
                              const Color &ColorC) = 0;

  virtual void onClearColor(const Color &color) = 0;
};

}

#endif //TINYRENDER_INCLUDE_CANVAS_HPP_
