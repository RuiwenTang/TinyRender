#pragma once

#include <TGAImage.h>
#include <Geometry.h>

namespace TRM
{

class TinyRender
{
  public:
    TinyRender(): mImage(nullptr), mZbuffer(nullptr), mWidth(0), mHeight(0) { }
	TinyRender(TGAImage* image): mImage(image),mZbuffer(nullptr), mWidth(0), mHeight(0) {
        if (mImage != nullptr) {
            attachBuffer(mImage);
        }
    }
    virtual ~TinyRender() {
        if (mZbuffer != nullptr) {
            delete[] mZbuffer;
        }
    }

    void attachBuffer(TGAImage* image);
    void triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c, const TGAColor& color);
	void triangle(Vec2i* pts, TGAColor* colors);
	void triangle(Vec3f* pts, Vec2f* uvs, TGAImage* texture);

  protected:
    TGAImage* mImage;
    float* mZbuffer;
    int mWidth;
    int mHeight;
};

}
