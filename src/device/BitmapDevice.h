#pragma once

#include <Device.h>

#include <vector>

namespace TRM {

class BitmapDevice : public Device {
 public:
  BitmapDevice(std::shared_ptr<Bitmap> bitmap);
  ~BitmapDevice() override = default;

  void UpdateMatrix(Matrix const& matrix) override;

  void Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
            Color const& color) override;

  void Triangle(Vec2i const& a, Vec2i const& b, Vec2i const& c, Color const& ca,
                Color const& cb, Color const& cc) override;

  void Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c, Color const& ca,
                Color const& cb, Color const& cc) override;

  void Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
                Vec2f const& uva, Vec2f const& uvb, Vec2f uvc,
                std::shared_ptr<Bitmap> const& texture) override;

  void Flush() override;

 private:
  std::shared_ptr<Bitmap> mBitmap;
  std::vector<float> mZBuffer;
  Matrix mMatrix;
};

}  // namespace TRM