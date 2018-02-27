#pragma once

#include <TGAImage.h>
#include <Geometry.h>

namespace TRM
{

class TinyRender
{
  public:
    TinyRender(): mImage(nullptr) { }
	TinyRender(TGAImage* image): mImage(image) { }
    virtual ~TinyRender() { }

    void attachBuffer(TGAImage* image);
    void triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c, const TGAColor& color);
	void triangle(Vec2i* pts, TGAColor* colors);
	void triangle(Vec3f* pts, Vec2f* uvs, TGAImage* texture);

  protected:
    TGAImage* mImage;
};

}
