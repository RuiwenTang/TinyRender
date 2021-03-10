#pragma once

#include <Device.h>
#include <Geometry.h>
#include <TGAImage.h>

#include <memory>

namespace TRM {

class TinyRender3D {
 public:
  TinyRender3D(std::unique_ptr<Device> device) : mDevice(std::move(device)) {}
  ~TinyRender3D() = default;

  void SetViewMatrix(Matrix const& matrix);
  void SetPerspectiveMatrix(Matrix const& Matrix);
  void DrawLine(int x0, int y0, int x1, int y1, Color const& color);
  void DrawTriangle(Vec2i const& a, Vec2i const& b, Vec2i const& c,
                    Color const& color);
  void DrawTriangle(Vec2i* pts, Color* colors);
  void DrawTriangle(Vec3f* pts, Color* colors);
  void DrawTriangle(Vec3f* pts, Vec2f* uvs, Bitmap* texture);

 private:
  std::unique_ptr<Device> mDevice;
};

class [[deprecated("Use TinyRender3D instead")]] TinyRender {
 public:
  TinyRender() : mImage(nullptr), mZBuffer(nullptr) {}
  TinyRender(TGAImage * image) : mImage(nullptr), mZBuffer(nullptr) {
    attachBuffer(image);
  }
  virtual ~TinyRender() {}

  void attachBuffer(TGAImage * image);
  void line(int x0, int y0, int x1, int y1, const TGAColor& color);
  void triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c,
                const TGAColor& color);
  void triangle(Vec2i * pts, TGAColor * colors);
  void triangle(Vec3f * pts, TGAColor * colors);
  void triangle(Vec3f * pts, Vec2f * uvs, TGAImage * texture);

 protected:
  TGAImage* mImage;
  float* mZBuffer;
};

}  // namespace TRM
