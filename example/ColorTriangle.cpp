#include <Bitmap.h>
#include <Color.h>
#include <Geometry.h>
#include <TinyRender.h>

#include <iostream>
#include <memory>

using namespace TRM;

int main(int argc, const char** argv) {
  std::shared_ptr<Bitmap> image = std::make_shared<Bitmap>(800, 600);
  TinyRender3D render{Device::CreateBitmapDevice(image)};
  Vec2i a, b, c;
  a[0] = 400;
  a[1] = 100;
  b[0] = 100;
  b[1] = 500;
  c[0] = 700;
  c[1] = 500;

  Vec2i pts[] = {a, b, c};
  Color colors[] = {Color::ColorRed(), Color::ColorGreen(), Color::ColorBlue()};
  
  render.DrawTriangle(pts, colors);

  image->WriteToPng("colorBuffer.png");

  return 0;
}
