#include <RenderBuffer.h>

namespace TRM {

void RenderBuffer::attachImage(TGAImage *image) {
  mImage = image;
}

void RenderBuffer::clear(const Color &color) {
  for (int x = 0; x < this->mImage->get_width(); x++) {
    for (int y = 0; y < this->mImage->get_height(); y++) {
      mImage->set(x, y, color);
    }
  }
}

void RenderBuffer::triangle(const Vec2i &a, const Vec2i &b, const Vec2i &c, const Color &color) {

}

void RenderBuffer::triangle(const Vec2i &a,
                            const Vec2i &b,
                            const Vec2i &c,
                            const Color &colorA,
                            const Color &colorB,
                            const Color &colorC) {
  Vec2i bbox_max{0, 0};
  Vec2i bbox_min{this->mImage->get_width() - 1, this->mImage->get_height() - 1};

  // bounding box max x
  bbox_max[0] = std::min(this->mImage->get_width() - 1, std::max(a[0], std::max(b[0], c[0])));
  bbox_max[1] = std::min(this->mImage->get_height() - 1, std::max(a[1], std::max(b[1], c[1])));

  bbox_min[0] = std::max(0, std::min(a[0], std::min(b[0], c[0])));
  bbox_min[1] = std::max(0, std::min(a[1], std::min(b[1], c[1])));

  for (int x = bbox_min[0]; x < bbox_max[0]; x++) {
    for (int y = bbox_min[1]; y < bbox_max[1]; y++) {
      auto bc_res = barycentric(a, b, c, Vec2i{x, y});
      bool outside = bc_res[0] < 0 || bc_res[1] < 0 || bc_res[2] < 0;
      if (outside) {
        continue;
      }

      Color out_color;
      out_color.bgra[0] = bc_res[0] * colorA.bgra[0] + bc_res[1] * colorB.bgra[0] + bc_res[2] * colorC.bgra[0];
      out_color.bgra[1] = bc_res[0] * colorA.bgra[1] + bc_res[1] * colorB.bgra[1] + bc_res[2] * colorC.bgra[1];
      out_color.bgra[2] = bc_res[0] * colorA.bgra[2] + bc_res[1] * colorB.bgra[2] + bc_res[2] * colorC.bgra[2];

      mImage->set(x, y, out_color);
    }
  }
}

void RenderBuffer::drawLine(const Vec2i &a, const Vec2i &b, int lineWidth, const Color &color) {
  Vec2f p_ab {b[0] - a[0], b[1] - a[1]};

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

 Vec2i ta {a[0] + p_up[0], a[1] + p_up[1]};
 Vec2i tc {a[0] - p_up[0], a[1] - p_up[1]};
 Vec2i tb {b[0] + p_up[0], b[1] + p_up[1]};
 Vec2i td {b[0] - p_up[0], b[1] - p_up[1]};

 triangle(ta, tc, td, color, color, color);
 triangle(ta, td, tb, color, color, color);
}

}