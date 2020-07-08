#pragma once

#include <TGAImage.h>
#include <Geometry.h>

namespace TRM {

class RenderBuffer {
 public:
  RenderBuffer() : mImage(nullptr) {}
  virtual ~RenderBuffer() = default;

  void attachImage(TGAImage *image);
  void clear(const TGAColor& color);
  void triangle(const Vec2i &a, const Vec2i &b, const Vec2i &c, const TGAColor &color);
  void triangle(const Vec2i &a,
                const Vec2i &b,
                const Vec2i &c,
                const TGAColor &colorA,
                const TGAColor &colorB,
                const TGAColor &colorC);

  void drawLine(const Vec2i& a, const Vec2i&b, int lineWidth, const TGAColor& color);

 protected:
  TGAImage *mImage;
};

}
