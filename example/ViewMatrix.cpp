#include <Model.h>
#include <Bitmap.h>
#include <TinyRender.h>
#include <config.h>
#include <Device.h>

#include <string>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {
  std::shared_ptr<Bitmap> framebuffer = std::make_shared<Bitmap>(WIDTH, HEIGHT);
  framebuffer->ClearWithColor(Color(0, 0, 0));

  TinyRender3D render(Device::CreateBitmapDevice(framebuffer));

  std::string model_path = ASSETS_ROOT + "/african_head.obj";
  std::string texture_path = ASSETS_ROOT + "/african_head_diffuse.tga";

  Model model(model_path.c_str());

  std::shared_ptr<Bitmap> texture = std::make_shared<Bitmap>(0, 0);
  texture->ReadFromFile(texture_path.c_str());

  Vec3f eye;
  eye[0] = 2.f;
  eye[1] = 2.f;
  eye[2] = 2.f;

  Vec3f center;
  center[0] = 0.f;
  center[1] = 0.f;
  center[2] = 0.f;
  TRM::Vec3f up;
  up[0] = 0.0f;
  up[1] = 1.f;
  up[2] = 0.f;

  Matrix viewMatrix = lookAt(eye, center, up);

  /** draw all vertex triangle ***/
  for (size_t i = 0; i < model.nfaces(); i++) {
    std::vector<int> face = model.face(i);
    Vec3f triangle[3];
    Vec3f vectors[3];
    for (size_t j = 0; j < 3; j++) {
      Vec3f c = model.vert(face[j]);
      // Vec4f cp;
      // cp[0] = c[0];
      // cp[1] = c[1];
      // cp[2] = c[2];
      // cp[3] = 1.f;
      // Vec4f pr = viewMatrix * cp;
      // c[0] = pr[0];
      // c[1] = pr[1];
      // c[2] = pr[2] * -1.f;

      vectors[j] = c;
      c[0] = (c[0] + 1.f) * WIDTH / 2.f;
      c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
      triangle[j][0] = c[0];
      triangle[j][1] = c[1];
      triangle[j][2] = c[2];
    }

    Vec2f uv[] = {model.uv(i, 0), model.uv(i, 1), model.uv(i, 2)};
    // render.DrawTriangle(triangle, uv, texture.get());
  }

  framebuffer->WriteToPng("framebuffer.png");

  return 0;
}
