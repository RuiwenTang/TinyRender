#include <Bitmap.h>
#include <Device.h>
#include <Geometry.h>
#include <Model.h>
#include <TinyRender.h>
#include <config.h>

#include <string>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

static const Color WHITE(255, 255, 255);

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {
  std::shared_ptr<Bitmap> framebuffer = std::make_shared<Bitmap>(WIDTH, HEIGHT);

  TinyRender3D render(Device::CreateBitmapDevice(framebuffer));
  framebuffer->ClearWithColor(Color::ColorBlack());

  std::string model_path = ASSETS_ROOT + "/african_head.obj";

  Model model(model_path.c_str());
  /** draw all vertex triangle ***/
  for (size_t i = 0; i < model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    Vec3f triangle[3];
    for (size_t j = 0; j < 3; j++) {
      Vec3f c = model.vert(face[j]);
      c[0] = (c[0] + 1.f) * WIDTH / 2.f;
      c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
      triangle[j] = c;
    }
    render.DrawLine(triangle[0][0], triangle[0][1], triangle[1][0],
                    triangle[1][1], WHITE);
    render.DrawLine(triangle[1][0], triangle[1][1], triangle[2][0],
                    triangle[2][1], WHITE);
    render.DrawLine(triangle[2][0], triangle[2][1], triangle[0][0],
                    triangle[0][1], WHITE);
  }

  framebuffer->FlipVertically();
  framebuffer->WriteToPng("framebuffer.png");

  return 0;
}
