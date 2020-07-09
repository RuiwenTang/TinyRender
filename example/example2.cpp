//
// Created by TangRuiwen on 2020/7/8.
//

#include "TGAImage.h"
#include "Geometry.h"
#include "RenderBuffer.h"
#include <iostream>

using namespace TRM;

int main(int argc, const char **argv) {
  Color white{255, 255, 255};
  Color red{255, 0, 0};
  Color green {0, 255, 0};
  Color blue {0, 0, 255};

  TGAImage image{800, 600, TGAImage::RGB};
  RenderBuffer render_buffer;
  render_buffer.attachImage(std::addressof(image));

  render_buffer.clear(white);

  render_buffer.drawLine(Vec2i{200, 200}, Vec2i{400, 300}, 20, blue);
  render_buffer.drawLine(Vec2i{400, 300}, Vec2i {320, 220}, 20, blue);
  render_buffer.triangle(Vec2i{200, 200}, Vec2i{200, 400}, Vec2i{400, 400}, red, green, blue);


  image.write_tga_file("frame_buffer.tga");

  return 0;
}