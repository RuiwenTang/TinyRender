#include <TinyRender.h>

namespace TRM {

    
vec<3, float> barycentric(const vec<3, float>& a, const vec<3, float>& b, const vec<3, float>& c, const vec<2, int>& p) {

    vec<3, float> temp;
    temp[0] = float(p[0]);
    temp[1] = float(p[1]);
        
    return barycentric(a, b, c, temp);
}
    
void TinyRender::attachBuffer(TGAImage* image) {
    mImage = image;
    if (mImage != nullptr && mImage->get_width() > 0 && mImage->get_height() > 0) {
        if (mZBuffer != nullptr) {
            delete[] mZBuffer;
            mZBuffer = nullptr;
        }
        
        int width = mImage->get_width();
        int height = mImage->get_height();
        mZBuffer = new float[width * height];
        for(size_t i = 0; i < width * height; i++) {
            mZBuffer[i] = -std::numeric_limits<float>::max();
        }
    }
}

    void TinyRender::line(int x0, int y0, int x1, int y1, const TRM::TGAColor &color) {
        bool steep = false;
        if (std::abs(x0-x1)<std::abs(y0-y1)) {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0>x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx = x1-x0;
        int dy = y1-y0;
        int derror2 = std::abs(dy)*2;
        int error2 = 0;
        int y = y0;
        for (int x=x0; x<=x1; x++) {
            if (steep) {
                mImage->set(y, x, color);
            } else {
                mImage->set(x, y, color);
            }
            error2 += derror2;
            if (error2 > dx) {
                y += (y1>y0?1:-1);
                error2 -= dx*2;
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
    
    int width = mImage->get_width();

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

void TinyRender::triangle(Vec3f *pts, TGAColor *colors) {
    Vec2i leftTop, rightBottom;

    int width = mImage->get_width();

    Vec2i ptis[3];
    for (size_t i = 0; i < 3; i++) {
        ptis[i][0] = int(pts[i][0] + .5f);
        ptis[i][1] = int(pts[i][1] + .5f);
    }

    bbox(leftTop, rightBottom, ptis);

    Vec2i p;
    for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
        for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
            Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], p);
            if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

            float pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] + pts[2][2] * bc_screen[2];

            if (mZBuffer[ p[1] * width + p[0] ] > pz) continue;


            mZBuffer[ p[1] * width + p[0] ] = pz;

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
    
    int width = mImage->get_width();
    
	Vec2i ptis[3];
	for (size_t i = 0; i < 3; i++) {
		ptis[i][0] = int(pts[i][0] + .5f);
		ptis[i][1] = int(pts[i][1] + .5f);
	}

	bbox(leftTop, rightBottom, ptis);

	Vec2i p;
	for (p[0] = leftTop[0]; p[0] <= rightBottom[0]; p[0]++) {
		for (p[1] = leftTop[1]; p[1] <= rightBottom[1]; p[1]++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
            
            float pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] + pts[2][2] * bc_screen[2];
            
            if (mZBuffer[ p[1] * width + p[0] ] > pz) continue;
            
            
            mZBuffer[ p[1] * width + p[0] ] = pz;
			/**
			 * Calculate texture color
			**/
			Vec2f uvf = uvs[0] * bc_screen[0] + uvs[1] * bc_screen[1] + uvs[2] * bc_screen[2];
			TGAColor c = texture->get(int((texture->get_width()-1) * uvf[0] + .5f), int((texture->get_height()-1) * uvf[1] + .5f));
			mImage->set(p[0], p[1], c);
		}
	}
}

}
