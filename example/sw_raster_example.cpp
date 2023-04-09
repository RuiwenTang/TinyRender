#include <Bitmap.h>
#include <SWRaster.h>

#include <iostream>
#include <memory>

#define WIDTH 800
#define HEIGHT 600

struct QuadCoeff {
  TRM::Vec2f A = {};
  TRM::Vec2f B = {};
  TRM::Vec2f C = {};

  QuadCoeff(TRM::Vec2f const& a, TRM::Vec2f const& b, TRM::Vec2f const& c) {
    C = a;
    auto p1 = b;
    auto p2 = c;
    B = (p1 - C) * 2.f;
    A = p2 - p1 * 2.f + C;
  }

  TRM::Vec2f eval(float t) { return eval({t, t}); }

  TRM::Vec2f eval(TRM::Vec2f const& tt) { return (A * tt + B) * tt + C; }
};

int main(int argc, const char** argv) {
  std::shared_ptr<TRM::Bitmap> framebuffer =
      std::make_shared<TRM::Bitmap>(WIDTH, HEIGHT);

  framebuffer->ClearWithColor(TRM::Color(255, 255, 255, 255));

  TRM::SWRaster raster{};

  // int x1 = 10;
  // int y1 = 10;
  // int x2 = 150;
  // int y2 = 200;
  // int x3 = 300;
  // int y3 = 100;

  // raster.move_to(x1, y1);
  // raster.line_to(x2, y2);
  // raster.line_to(x3, y3);
  // raster.line_to(x1, y1);

  // star
  // raster.move_to(100, 10);
  // raster.line_to(40, 180);
  // raster.line_to(190, 60);
  // raster.line_to(10, 60);
  // raster.line_to(160, 180);
  // raster.line_to(100, 10);

  raster.move_to(10, 10);
  raster.quad_to(160, 10, 160, 160);

  raster.line_to(10, 10);

  raster.sweep();

  auto const& spans = raster.current_spans();
  for (auto const& span : spans) {
    int32_t x = span.x;
    int32_t y = span.y;
    for (int32_t i = 0; i < span.len; i++) {
      framebuffer->BlendPixel(x + i, y, TRM::Color(0, 0, 0, span.cover));
    }
  }

  framebuffer->WriteToPng("sw_raster_test.png");
  return 0;
}
