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

  std::vector<glm::vec2> point_stack{};

  // at begining upper and lower chain share a common start point
  point_stack.emplace_back(upper_chain.front().start);

  bool upper = true;
  int32_t upper_chain_index = 0;
  int32_t lower_chain_index = 0;

  // lower chain first
  if (upper_chain.front().end.x > lower_chain.front().end.x) {
    upper = false;
    lower_chain_index++;
    point_stack.emplace_back(lower_chain.front().end);
  } else {
    upper_chain_index++;
    point_stack.emplace_back(upper_chain.front().end);
  }

  while (upper_chain_index < upper_chain.size() &&
         lower_chain_index < lower_chain.size()) {
    bool next_is_upper = true;
    if (upper_chain[upper_chain_index].end.x >
        lower_chain[lower_chain_index].end.x) {
      next_is_upper = false;
    }

    if (upper == next_is_upper) {
      // check if next point only visible one point
      glm::vec2 p;
      if (upper) {  // next point is upper chain
        p = upper_chain[upper_chain_index].end;

        if (point_stack.size() <= 1) {
          point_stack.emplace_back(p);
          upper_chain_index++;
          continue;
        }

        float prev_k =
            calculate_k(point_stack[point_stack.size() - 2],
                        point_stack[point_stack.size() - 1], -99999.f);

        float current_k =
            calculate_k(point_stack[point_stack.size() - 1], p, -99999.f);

        if (current_k <= prev_k) {
          point_stack.emplace_back(p);
          upper_chain_index++;
          continue;
        }

        do {
          // just draw line triangle

          render.DrawLine(p.x, p.y, point_stack.back().x, point_stack.back().y,
                          Color::ColorGreen());
          render.DrawLine(p.x, p.y, point_stack[point_stack.size() - 2].x,
                          point_stack[point_stack.size() - 2].y,
                          Color::ColorGreen());

          render.DrawLine(point_stack.back().x, point_stack.back().y,
                          point_stack[point_stack.size() - 2].x,
                          point_stack[point_stack.size() - 2].y,
                          Color::ColorGreen());

          point_stack.pop_back();

          if (point_stack.size() < 2) {
            break;
          }

          prev_k = calculate_k(point_stack[point_stack.size() - 2],
                               point_stack[point_stack.size() - 1], -99999.f);
          current_k = calculate_k(point_stack.back(), p, -99999.f);

          if (current_k <= prev_k) {
            break;
          }

        } while (point_stack.size() >= 2);

        point_stack.push_back(p);
        upper_chain_index++;
        continue;

      } else {  // next point is lower chain
        p = lower_chain[lower_chain_index].end;

        if (point_stack.size() <= 1) {
          point_stack.emplace_back(p);
          lower_chain_index++;
          continue;
        }

        float prev_k =
            calculate_k(point_stack[point_stack.size() - 2],
                        point_stack[point_stack.size() - 1], 99999.f);

        float current_k =
            calculate_k(point_stack[point_stack.size() - 1], p, 99999.f);

        if (current_k >= prev_k) {
          point_stack.emplace_back(p);
          lower_chain_index++;
          continue;
        }

        do {
          render.DrawLine(p.x, p.y, point_stack.back().x, point_stack.back().y,
                          Color::ColorGreen());
          render.DrawLine(p.x, p.y, point_stack[point_stack.size() - 2].x,
                          point_stack[point_stack.size() - 2].y,
                          Color::ColorGreen());

          render.DrawLine(point_stack.back().x, point_stack.back().y,
                          point_stack[point_stack.size() - 2].x,
                          point_stack[point_stack.size() - 2].y,
                          Color::ColorGreen());

          point_stack.pop_back();

          if (point_stack.size() < 2) {
            break;
          }

          float prev_k =
              calculate_k(point_stack[point_stack.size() - 2],
                          point_stack[point_stack.size() - 1], 99999.f);

          float current_k =
              calculate_k(point_stack[point_stack.size() - 1], p, 99999.f);

          if (current_k >= prev_k) {
            break;
          }

        } while (point_stack.size() >= 2);

        point_stack.emplace_back(p);
        lower_chain_index++;
        continue;
      }
    } else {  // next point is in opposite
      // first triangle all remin points with p
      glm::vec2 p;
      if (next_is_upper) {
        p = upper_chain[upper_chain_index].end;
      } else {
        p = lower_chain[lower_chain_index].end;
      }

      while (point_stack.size() > 1) {
        render.DrawLine(point_stack[0].x, point_stack[0].y, point_stack[1].x,
                        point_stack[1].y, Color::ColorGreen());

        render.DrawLine(p.x, p.y, point_stack[0].x, point_stack[0].y,
                        Color::ColorGreen());

        render.DrawLine(p.x, p.y, point_stack[1].x, point_stack[1].y,
                        Color::ColorGreen());

        point_stack.erase(point_stack.begin());
      }

      point_stack.emplace_back(p);

      if (next_is_upper) {
        upper_chain_index++;
        upper = true;
      } else {
        upper = false;
        lower_chain_index++;
      }
    }
  }

  framebuffer->WriteToPng("triangulation_example_result.png");

  return 0;
}
