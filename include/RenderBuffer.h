#pragma once

#include <TGAImage.h>
#include <Geometry.h>

namespace TRM {

class RenderBuffer {
 public:
  RenderBuffer() : mImage(nullptr) {}
  virtual ~RenderBuffer() = default;

  void attachImage(TGAImage *image);
  void clear(const Color& color);
  void triangle(const Vec2i &a, const Vec2i &b, const Vec2i &c, const Color &color);
  void triangle(const Vec2i &a,
                const Vec2i &b,
                const Vec2i &c,
                const Color &colorA,
                const Color &colorB,
                const Color &colorC);

  void drawLine(const Vec2i& a, const Vec2i&b, int lineWidth, const Color& color);

 protected:
  TGAImage *mImage;
};

}
