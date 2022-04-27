#include <Bitmap.h>
#include <TinyRender.h>
#include <Triangulation.h>

#include <algorithm>
#include <glm/gtx/io.hpp>
#include <iostream>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

float calculate_k(glm::vec2 const& p1, glm::vec2 const& p2, float nan_value) {
  if (p1.x == p2.x) {
    return nan_value;
  }

  return (p2.y - p1.y) / (p2.x - p1.x);
}

std::unique_ptr<EdgeList> build_mesh() {
  auto list = std::make_unique<EdgeList>();

  std::vector<glm::vec2> pts{
      {199.f, 34.f},  {253.f, 143.f}, {374.f, 160.f}, {287.f, 244.f},
      {244.f, 374.f}, {200.f, 280.f}, {150.f, 374.f}, {112.f, 245.f},
      {26.f, 161.f},  {146.f, 143.f}, {199.f, 34.f},
  };

  for (size_t i = 0; i < pts.size() - 1; i++) {
    list->add_edge(Edge(pts[i], pts[i + 1]));
  }

  return list;
}

int main(int argc, const char** argv) {
  auto edge_list = build_mesh();

  edge_list->sort_edge_list();

  std::vector<Edge> upper_chain{};
  std::vector<Edge> lower_chain{};

  glm::vec2 upper_current;
  glm::vec2 lower_current;

  bool first = true;

  for (auto const& e : edge_list->raw_list()) {
    std::cout << "e { " << e.start << " -> " << e.end
              << " } winding = " << e.winding << std::endl;
    if (first) {
      if (e.end.y < e.start.y) {
        upper_chain.emplace_back(e);
        upper_current = e.end;
        lower_current = e.start;
      } else {
        lower_chain.emplace_back(e);
        upper_current = e.start;
        lower_current = e.end;
      }

      first = false;
    } else {
      if (e.start == upper_current) {
        upper_chain.emplace_back(e);
        upper_current = e.end;
      } else {
        lower_chain.emplace_back(e);
        lower_current = e.end;
      }
    }
  }

  std::cout << "min_x -> max_x end" << std::endl;

  std::shared_ptr<Bitmap> framebuffer = std::make_shared<Bitmap>(WIDTH, HEIGHT);
  framebuffer->ClearWithColor(Color::ColorWhite());

  TinyRender3D render(Device::CreateBitmapDevice(framebuffer));

  auto const& edges = edge_list->raw_list();

  for (auto const& e : upper_chain) {
    render.DrawLine(e.start.x, e.start.y, e.end.x, e.end.y, Color::ColorRed());
  }

  for (auto const& e : lower_chain) {
    render.DrawLine(e.start.x, e.start.y, e.end.x, e.end.y, Color::ColorBlue());
  }

  framebuffer->WriteToPng("triangulation_example.png");

  framebuffer->ClearWithColor(Color::ColorWhite());

  Triangulation triangulation{upper_chain, lower_chain};

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
