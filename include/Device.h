#pragma once

#include <Bitmap.h>
#include <Color.h>
#include <Geometry.h>

#include <memory>

namespace TRM {

class Device {
 public:
  virtual ~Device() = default;

  virtual void UpdatePerspectiveMatrix(Matrix const& matrix) = 0;
  virtual void UpdateViewMatrix(Matrix const& matrix) = 0;

  virtual void Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                    Color const& color) = 0;

  virtual void Triangle(Vec2i const& a, Vec2i const& b, Vec2i const& c,
                        Color const& ca, Color const& cb, Color const& cc) = 0;

  virtual void Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
                        Color const& ca, Color const& cb, Color const& cc) = 0;

  virtual void Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
                        Vec2f const& uva, Vec2f const& uvb, Vec2f uvc,
                        std::shared_ptr<Bitmap> const& texture) = 0;

  void Triangle(Vec2i const& a, Vec2i const& b, Vec2i const& c,
                Color const& color) {
    Triangle(a, b, c, color, color, color);
  }
};

}  // namespace TRM