#include <Geometry.h>
#include <TGAImage.h>
#include <TinyRender.h>

#include <iostream>


using namespace TRM;

static TGAColor WHITE(255, 255, 255);
static TGAColor RED(255, 0, 0);
static TGAColor GREEN(0, 255, 0);
static TGAColor BLUE(0, 0, 255);

int main(int argc, const char** argv) {
  TGAImage image(800, 600, TGAImage::RGB);
  image.clearColor(WHITE);
  TinyRender render;
  render.attachBuffer(&image);

  Vec2i a, b, c;
  a[0] = 400;
  a[1] = 100;
  b[0] = 100;
  b[1] = 500;
  c[0] = 700;
  c[1] = 500;

  Vec2i pts[] = {a, b, c};
  TGAColor colors[] = {RED, GREEN, BLUE};
  render.triangle(pts, colors);

  image.write_png_stb("colorBuffer.png");

  return 0;
}
