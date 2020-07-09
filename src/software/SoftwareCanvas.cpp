//
// Created by TangRuiwen on 2020/7/9.
//

#include "SoftwareCanvas.hpp"

namespace TRM {

SoftwareCanvas::SoftwareCanvas(TGAImage* image)
    : Canvas(), mImage_(image) {}

void SoftwareCanvas::onDrawTriangle(const TRM::Vec2i &a,
                                    const TRM::Vec2i &b,
                                    const TRM::Vec2i &c,
                                    const TRM::Color &colorA,
                                    const TRM::Color &colorB,
                                    const TRM::Color &colorC) {

  Vec2i bbox_max{0, 0};
  Vec2i bbox_min{this->mImage_->get_width() - 1, this->mImage_->get_height() - 1};

  // bounding box max x
  bbox_max[0] = std::min(this->mImage_->get_width() - 1, std::max(a[0], std::max(b[0], c[0])));
  bbox_max[1] = std::min(this->mImage_->get_height() - 1, std::max(a[1], std::max(b[1], c[1])));

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

      mImage_->set(x, y, out_color);
    }
  }
}
void SoftwareCanvas::onClearColor(const Color &color) {
  for (int x = 0; x < this->mImage_->get_width(); x++) {
    for (int y = 0; y < this->mImage_->get_height(); y++) {
      mImage_->set(x, y, color);
    }
  }
}

}
