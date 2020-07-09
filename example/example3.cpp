//
// Created by TangRuiwen on 2020/7/9.
//

#include "Canvas.hpp"
#include "TGAImage.h"
#include "Color.hpp"

using namespace TRM;

int main(int argc, const char** argv) {

  TGAImage image{800, 600, TGAImage::RGB};

  image.clear();

  auto canvas = Canvas::MakeRasterDirect(std::addressof(image));
  canvas->clear(Color::WHITE);

  canvas->drawLine(200, 200, 400, 300, 5, Color::BLUE);
  canvas->drawLine(400, 300, 320, 220, 5, Color::BLUE);


  canvas->drawCircle(300, 300, 80, Color::RED);

  image.write_tga_file("frame_buffer.tga");
  return 0;
}