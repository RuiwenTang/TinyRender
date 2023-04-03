#include <Bitmap.h>
#include <TinyRender.h>
#include <Triangulation.h>

#include <algorithm>
#include <glm/gtx/io.hpp>
#include <iostream>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

std::vector<glm::vec2> build_mesh() {

  std::vector<glm::vec2> pts{
      {199.f, 34.f},  {253.f, 143.f}, {374.f, 160.f}, {287.f, 244.f},
      {244.f, 374.f}, {200.f, 280.f}, {150.f, 374.f}, {112.f, 245.f},
      {26.f, 161.f},  {146.f, 143.f}, {199.f, 34.f},
  };

  return pts;
}

int main(int argc, const char** argv) {
  auto edge_list = build_mesh();

  std::shared_ptr<Bitmap> framebuffer = std::make_shared<Bitmap>(WIDTH, HEIGHT);
  framebuffer->ClearWithColor(Color::ColorWhite());

  TinyRender3D render(Device::CreateBitmapDevice(framebuffer));

  for (size_t i = 0; i < edge_list.size(); i++) {
    if (i == edge_list.size() - 1) {
      render.DrawLine(edge_list[i].x, edge_list[i].y, edge_list[0].x,
                      edge_list[0].y, Color::ColorGreen());
    } else {
      render.DrawLine(edge_list[i].x, edge_list[i].y, edge_list[i + 1].x,
                      edge_list[i + 1].y, Color::ColorGreen());
    }
  }

  framebuffer->WriteToPng("triangulation_example.png");

  framebuffer->ClearWithColor(Color::ColorWhite());

  Triangulation triangulation{};

  triangulation.add_path(edge_list);

  triangulation.do_triangulate([ren = &render](glm::vec2 const& p1,
                                               glm::vec2 const& p2,
                                               glm::vec2 const& p3) {
    ren->DrawLine(p1.x, p1.y, p2.x, p2.y, Color::ColorGreen());
    ren->DrawLine(p1.x, p1.y, p3.x, p3.y, Color::ColorGreen());
    ren->DrawLine(p2.x, p2.y, p3.x, p3.y, Color::ColorGreen());
  });

  framebuffer->WriteToPng("triangulation_example_result.png");

  return 0;
}
