#pragma once

#include <TGAImage.h>
#include <Geometry.h>

namespace TRM
{

class TinyRender
{
  public:
    TinyRender(): mImage(nullptr) { }
    virtual ~TinyRender() { }

    void attachBuffer(TGAImage* image);
    void triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c, const TGAColor& color);


  protected:
    TGAImage* mImage;
};

}
