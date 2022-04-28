#include <Bitmap.h>
#include <TinyRender.h>
#include <Triangulation.h>

#include <glm/gtx/io.hpp>
#include <iostream>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

std::unique_ptr<EdgeList> build_mesh() {
  auto list = std::make_unique<EdgeList>();

  std::vector<glm::vec2> pts{
      {199.f, 34.f},  {253.f, 143.f}, {374.f, 160.f}, {287.f, 244.f},
      {307.f, 365.f}, {199.f, 309.f}, {97.f, 365.f},  {112.f, 245.f},
      {26.f, 161.f},  {146.f, 143.f}, {199.f, 34.f},
  };
  // std::vector<glm::vec2> pts{
  //     {100, 100}, {300, 100}, {300, 300}, {100, 300}, {100, 100}};

  for (size_t i = 0; i < pts.size() - 1; i++) {
    list->add_edge(Edge(pts[i], pts[i + 1]));
  }

  return list;
}

int main(int argc, const char** argv) {
  auto edge_list = build_mesh();

  edge_list->sort_edge_list();

  for (auto const& e : edge_list->raw_list()) {
    std::cout << "e { " << e.start << " -> " << e.end
              << " } winding = " << e.winding << std::endl;
  }

  return 0;
}