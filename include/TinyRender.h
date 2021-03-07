#pragma once

#include <Geometry.h>
#include <TGAImage.h>


namespace TRM {

class TinyRender {
 public:
  TinyRender() : mImage(nullptr), mZBuffer(nullptr) {}
  TinyRender(TGAImage* image) : mImage(nullptr), mZBuffer(nullptr) {
    attachBuffer(image);
  }
  virtual ~TinyRender() {}

  void attachBuffer(TGAImage* image);
  void line(int x0, int y0, int x1, int y1, const TGAColor& color);
  void triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c,
                const TGAColor& color);
  void triangle(Vec2i* pts, TGAColor* colors);
  void triangle(Vec3f* pts, TGAColor* colors);
  void triangle(Vec3f* pts, Vec2f* uvs, TGAImage* texture);

 protected:
  TGAImage* mImage;
  float* mZBuffer;
};

}  // namespace TRM
