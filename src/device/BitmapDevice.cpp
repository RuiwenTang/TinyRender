#include "BitmapDevice.h"

#include <array>

namespace TRM {

BitmapDevice::BitmapDevice(std::shared_ptr<Bitmap> bitmap)
    : Device(), mBitmap(std::move(bitmap)) {
  mMatrix = IdentityMatrix();
}

void BitmapDevice::UpdateMatrix(Matrix const& matrix) { mMatrix = matrix; }

void BitmapDevice::Triangle(const Vec2i& a, Vec2i const& b, Vec2i const& c,
                            Color const& ca, Color const& cb, Color const& cc) {
  Vec3f a3f{a.x, a.y, 0.f};
  Vec3f b3f{b.x, b.y, 0.f};
  Vec3f c3f{c.x, c.y, 0.f};

  Triangle(a3f, b3f, c3f, ca, cb, cc);
}

void BitmapDevice::Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
                            Color const& ca, Color const& cb, Color const& cc) {
  int32_t bitmap_width = mBitmap->width();
  int32_t bitmap_height = mBitmap->height();

  Vec2i bboxmin(bitmap_width - 1, bitmap_height - 1);
  Vec2i bboxmax(0, 0);
  Vec2i clamp(bitmap_width - 1, bitmap_height - 1);

  std::array<Vec3f, 3> pts;
  pts[0] = mMatrix * Vec4f{a.x, a.y, a.z, 1.f};
}

}  // namespace TRM