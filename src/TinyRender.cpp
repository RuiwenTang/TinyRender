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

void TinyRender::triangle(Vec2i* pts, TGAColor* colors) {
	Vec2i leftTop, rightBottom;

	bbox(leftTop, rightBottom, pts);

	Vec2i p;
	for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
		for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
			TGAColor c;
			/*
				barycentric result can also interpolation Color
			*/
			c.bgra[0] = bc_screen[0] * colors[0].bgra[0] + bc_screen[1] * colors[1].bgra[0] + bc_screen[2] * colors[2].bgra[0];
			c.bgra[1] = bc_screen[0] * colors[0].bgra[1] + bc_screen[1] * colors[1].bgra[1] + bc_screen[2] * colors[2].bgra[1];
			c.bgra[2] = bc_screen[0] * colors[0].bgra[2] + bc_screen[1] * colors[1].bgra[2] + bc_screen[2] * colors[2].bgra[2];
			mImage->set(p[0], p[1], c);
		}
	}
}

}