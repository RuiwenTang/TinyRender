#include <Bitmap.h>
#include <RasterizerCells.h>

#include <iostream>

#define UP_SCALE(x) (x << TRM::poly_subpixel_shift)

#define WIDTH 800
#define HEIGHT 600

int main(int argc, const char** argv) {
  std::shared_ptr<TRM::Bitmap> framebuffer =
      std::make_shared<TRM::Bitmap>(WIDTH, HEIGHT);

  framebuffer->ClearWithColor(TRM::Color(255, 255, 255, 255));

  TRM::RasterizerCells raster{};

  int x1 = 10;
  int y1 = 10;
  int x2 = 150;
  int y2 = 200;
  int x3 = 300;
  int y3 = 10;

  raster.reset();
  raster.line(UP_SCALE(x1), UP_SCALE(y1), UP_SCALE(x2), UP_SCALE(y2));
  raster.line(UP_SCALE(x2), UP_SCALE(y2), UP_SCALE(x3), UP_SCALE(y3));
  raster.line(UP_SCALE(x3), UP_SCALE(y3), UP_SCALE(x1), UP_SCALE(y1));

  raster.sort();

  auto const& cells = raster.getCells();

  for (auto const& cell : cells) {
    uint8_t alpha = raster.calculate_alpha(cell.area);
    std::cout << "{ " << cell.x << ", " << cell.y
              << " } alpha = " << (uint32_t)alpha << " "
              << std::endl;
  
    framebuffer->BlendPixel(cell.x, cell.y, TRM::Color(0, 0, 0, alpha));
  }


  framebuffer->WriteToPng("raster_test.png");
  return 0;
}