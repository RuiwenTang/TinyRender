//
// Created by TangRuiwen on 2020/7/9.
//

#include <cmath>
#include "Canvas.hpp"
#include "software/SoftwareCanvas.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace TRM {

#define CIRCLE_SEGMENT_COUNT 256


std::unique_ptr<Canvas> Canvas::MakeRasterDirect(TGAImage *image) {
  return std::unique_ptr<Canvas> {new SoftwareCanvas{image}};
}

void Canvas::drawLine(int x0, int y0, int x1, int y1, uint32_t lineWidth, const Color &color) {
  Vec2f p_ab{x1 - x0, y1 - y0};

  /**
   *   a ******** b
   *     *
   *       *
   *         *
   *           *
   *   c ******** d
   */
  Vec2f p_normal {-p_ab[1], p_ab[0]};
  p_normal = glm::normalize(p_normal);
  Vec2f p_up {lineWidth * p_normal[0] / 2, lineWidth * p_normal[1] / 2};

  Vec2i ta {x0 + p_up[0], y0 + p_up[1]};
  Vec2i tc {x0 - p_up[0], y0 - p_up[1]};
  Vec2i tb {x1 + p_up[0], y1 + p_up[1]};
  Vec2i td {x1 - p_up[0], y1 - p_up[1]};

  this->onDrawTriangle(ta, tc, td, color, color, color);
  this->onDrawTriangle(ta, td, tb, color, color, color);
}

void Canvas::drawCircle(int centerX, int centerY, int radius, const Color &color) {
  double segmentWidth = M_PI * 2 / (double) CIRCLE_SEGMENT_COUNT;
  double angle = 0.0;
  std::vector<Vec2i > vertices;
  for(int i = 1; i <= CIRCLE_SEGMENT_COUNT; i++) {
    Vec2i p1 {glm::cos(angle) * radius + centerX, glm::sin(angle) * radius + centerY};
    vertices.emplace_back(p1);
    angle += segmentWidth;
  }

  for (size_t i = 0; i < vertices.size(); i++) {
    if (i == vertices.size() - 1) {
      this->drawLine(vertices[i].x, vertices[i].y, vertices[0].x, vertices[0].y, 20, color);
    } else {
      this->drawLine(vertices[i].x, vertices[i].y, vertices[i + 1].x, vertices[i + 1].y, 20, color);
    }
  }
}

}