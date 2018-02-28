#include <TinyRender.h>

namespace TRM {
    
void TinyRender::attachBuffer(TGAImage* image) {
    mImage = image;
    if (mImage != nullptr && mImage->get_width() > 0 && mImage->get_height() > 0) {
        if (mZbuffer) {
            delete[] mZbuffer;
        }
        mWidth = mImage->get_width();
        mHeight = mImage->get_height();
        mZbuffer = new float[mWidth * mHeight];
        for(size_t i = 0; i < mWidth * mHeight; i++) {
            mZbuffer[i] = std::numeric_limits<float>::max();
        }
    }
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

void TinyRender::triangle(Vec3f* pts, Vec2f* uvs, TGAImage* texture) {
	Vec2i leftTop, rightBottom;

	Vec2i ptis[3];
	for (size_t i = 0; i < 3; i++) {
		ptis[i][0] = int(pts[i][0] + .5f);
		ptis[i][1] = int(pts[i][1] + .5f);
	}

	bbox(leftTop, rightBottom, ptis);

	Vec2i p;
    float pz;
	for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
		for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
			Vec3f bc_screen = barycentric(ptis[0], ptis[1], ptis[2], p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
            pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] * pts[2][2] * bc_screen[2];
			/**
			 * Calculate texture color
			**/
            if (mZbuffer[ p[1] * mWidth + p[0]] >= pz) {
                mZbuffer[ p[1] * mWidth + p[0]] = pz;
                Vec2f uvf = uvs[0] * bc_screen[0] + uvs[1] * bc_screen[1] + uvs[2] * bc_screen[2];
                TGAColor c = texture->get(int((texture->get_width() )* uvf[0] + .5f), int((texture->get_height() )* uvf[1] + .5f));
                mImage->set(p[0], p[1], c);
            }
		}
	}
}

}
