//
// Created by TangRuiwen on 2020/7/9.
//

#ifndef TINYRENDER_SRC_SOFTWARE_SOFTWARECANVAS_HPP_
#define TINYRENDER_SRC_SOFTWARE_SOFTWARECANVAS_HPP_

#include "Canvas.hpp"
#include "TGAImage.h"

namespace TRM {

class SoftwareCanvas: public Canvas {
 public:
  SoftwareCanvas(TGAImage* image);
  ~SoftwareCanvas() override = default;

 protected:
  void onDrawTriangle(const Vec2i &a,
                      const Vec2i &b,
                      const Vec2i &c,
                      const Color &colorA,
                      const Color &colorB,
                      const Color &ColorC) override;

  void onClearColor(const Color &color) override;
 private:
  TGAImage* mImage_;
};

}

#endif //TINYRENDER_SRC_SOFTWARE_SOFTWARECANVAS_HPP_
