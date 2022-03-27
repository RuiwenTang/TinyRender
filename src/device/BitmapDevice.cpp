#include "BitmapDevice.h"

#include <algorithm>
#include <array>
#include <limits>

#include "DDALine.h"

namespace TRM {

BitmapDevice::BitmapDevice(std::shared_ptr<Bitmap> bitmap)
    : Device(), mBitmap(std::move(bitmap)) {
  mMatrix = IdentityMatrix();
  mZBuffer.resize(mBitmap->width() * mBitmap->height());
  mZBuffer.assign(mZBuffer.size(), std::numeric_limits<float>::max());
}

void BitmapDevice::UpdateMatrix(Matrix const& matrix) { mMatrix = matrix; }

void BitmapDevice::Line(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                        Color const& color) {
  int32_t s_x0 = x0 * line_bresenham_interpolator::subpixel_scale;
  int32_t s_y0 = y0 * line_bresenham_interpolator::subpixel_scale;
  int32_t s_x1 = x1 * line_bresenham_interpolator::subpixel_scale;
  int32_t s_y1 = y1 * line_bresenham_interpolator::subpixel_scale;

  line_bresenham_interpolator li{s_x0, s_y0, s_x1, s_y1};

  uint32_t len = li.len();

  if (len == 0) {
    mBitmap->SetPixel(x0, y0, color);
    return;
  }

  if (li.is_ver()) {
    do {
      int32_t s_x = li.x2();
      int32_t s_y = li.y1();

      int32_t i_x = li.x2_hr() % li.subpixel_mask;
      int32_t i_y = li.y2_hr() % li.subpixel_mask;

      double alpha = std::sqrt(i_x * i_y  / (255.0 * 255.0)) * 255;

      auto rgba = color.rgba();
      mBitmap->SetPixel(s_x, s_y, Color(rgba[0], rgba[1], rgba[2], (uint8_t) alpha));

      li.v_step();
    } while (--len);
  } else {
    do {
      int32_t s_x = li.x1();
      int32_t s_y = li.y2();

      mBitmap->SetPixel(s_x, s_y, color);

      li.h_step();
    } while (--len);
  }
}

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
  std::array<Color, 3> colors{ca, cb, cc};

  pts[0] = mMatrix * Vec4f{a.x, a.y, a.z, 1.f};
  pts[1] = mMatrix * Vec4f{b.x, b.y, b.z, 1.f};
  pts[2] = mMatrix * Vec4f{c.x, c.y, c.z, 1.f};

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = (int)std::max(
          0.f, std::min(static_cast<float>(bboxmin[j]), pts[i][j]));
      bboxmax[j] =
          (int)std::min(static_cast<float>(clamp[j]),
                        std::max(static_cast<float>(bboxmax[j]), pts[i][j]));
    }
  }

  Vec3f p;
  for (p[0] = bboxmin.x; p[0] <= bboxmax.x; p[0]++) {
    for (p[1] = bboxmin.y; p[1] <= bboxmax.y; p[1]++) {
      Vec3f bc_screen = barycentric<3>(pts[0], pts[1], pts[2], p);
      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

      float pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] +
                 pts[2][2] * bc_screen[2];

      size_t zIndex = static_cast<size_t>(p.x + p.y * bitmap_width);
      if (mZBuffer[zIndex] < pz) continue;

      mZBuffer[zIndex] = pz;

      Color c;
      /*
          barycentric result can also interpolation Color
      */

      c.rgba()[0] = bc_screen[0] * colors[0].rgba()[0] +
                    bc_screen[1] * colors[1].rgba()[0] +
                    bc_screen[2] * colors[2].rgba()[0];
      c.rgba()[1] = bc_screen[0] * colors[0].rgba()[1] +
                    bc_screen[1] * colors[1].rgba()[1] +
                    bc_screen[2] * colors[2].rgba()[1];
      c.rgba()[2] = bc_screen[0] * colors[0].rgba()[2] +
                    bc_screen[1] * colors[1].rgba()[2] +
                    bc_screen[2] * colors[2].rgba()[2];
      c.rgba()[3] = bc_screen[0] * colors[0].rgba()[3] +
                    bc_screen[1] * colors[1].rgba()[3] +
                    bc_screen[2] * colors[2].rgba()[3];

      mBitmap->SetPixel(static_cast<uint32_t>(p.x), static_cast<uint32_t>(p.y),
                        c);
    }
  }
}

void BitmapDevice::Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
                            Vec2f const& uva, Vec2f const& uvb, Vec2f uvc,
                            std::shared_ptr<Bitmap> const& texture) {}

void BitmapDevice::Flush() {}

}  // namespace TRM