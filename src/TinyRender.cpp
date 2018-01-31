#include <TinyRender.h>

namespace TRM {

void TinyRender::attachBuffer(TGAImage* image) {
    mImage = image;
}


void TinyRender::triangle(const Vec2i& a, const Vec2i& b, const Vec2i& c, const TGAColor& color) {
	Vec2i leftTop, rightBottom;
	Vec2i pts[] = { a, b, c };
	bbox(leftTop, rightBottom, pts);

	Vec2i p;
	for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
		for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
			Vec3f bc_screen = barycentric(a, b, c, p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

			mImage->set(p[0], p[1], color);
		}
	}
}

}