#include <Bitmap.h>
#include <Device.h>
#include <Model.h>
#include <TinyRender.h>
#include <config.h>

#include <random>
#include <string>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

static const Color WHITE(255, 255, 255);

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {
  std::shared_ptr<Bitmap> framebuffer = std::make_shared<Bitmap>(WIDTH, HEIGHT);

  TinyRender3D render(Device::CreateBitmapDevice(framebuffer));

  std::string model_path = ASSETS_ROOT + "/african_head.obj";

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(128, 255);

  Model model(model_path.c_str());
  /** draw all vertex triangle ***/
  for (size_t i = 0; i < model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    Vec2i triangle[3];
    for (size_t j = 0; j < 3; j++) {
      Vec3f c = model.vert(face[j]);
      c[0] = (c[0] + 1.f) * WIDTH / 2.f;
      c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
      triangle[j][0] = (int)c[0];
      triangle[j][1] = (int)c[1];
    }
    Color randomColor(rand() % 255, rand() % 255, rand() % 255);
    Color colors[] = {randomColor, randomColor, randomColor};
    render.DrawTriangle(triangle, colors);
  }

  framebuffer->FlipVertically();
  framebuffer->WriteToPng("framebuffer.png");

  return 0;
}
