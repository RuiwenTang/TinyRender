#include <TinyRender.h>

namespace TRM {


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
            mZBuffer[i] = std::numeric_limits<float>::max();
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
	Vec2i pts[] = { a, b, c };
	
    Vec2i bboxmin(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

	Vec2i p;
	for (p[0] = bboxmin.x; p[0] <= bboxmax.x; p[0]++) {
		for (p[1] = bboxmin.y; p[1] <= bboxmax.y; p[1]++) {
			Vec3f bc_screen = barycentric<2>(a, b, c, p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

			mImage->set(p[0], p[1], color);
		}
	}
}

void TinyRender::triangle(Vec2i* pts, TGAColor* colors) {

	Vec2i bboxmin(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    Vec2i bboxmax(0, 0);
    Vec2i clamp(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    
    int width = mImage->get_width();

	Vec2i p;
	for (p[0] = bboxmin.x; p[0] <= bboxmax.x; p[0]++) {
		for (p[1] = bboxmin.y; p[1] <= bboxmax.y; p[1]++) {
			Vec3f bc_screen = barycentric<2>(pts[0], pts[1], pts[2], p);
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

    int width = mImage->get_width();

    Vec2i ptis[3];
    for (size_t i = 0; i < 3; i++) {
        ptis[i][0] = int(pts[i][0] + .5f);
        ptis[i][1] = int(pts[i][1] + .5f);
    }

    Vec2f bboxmin(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    Vec2f bboxmax(0, 0);
    Vec2f clamp(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

    Vec3f p;
    for (p[0] = bboxmin.x; p[0] <= bboxmax.x; p[0]++) {
        for (p[1] = bboxmin.y; p[1] <= bboxmax.y; p[1]++) {
            Vec3f bc_screen = barycentric<3>(pts[0], pts[1], pts[2], p);
            if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

            float pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] + pts[2][2] * bc_screen[2];

            if (mZBuffer[ int(p.x + p.y * mImage->get_width()) ] < pz) continue;


            mZBuffer[ int(p.x + p.y * mImage->get_width()) ] = pz;

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

    int width = mImage->get_width();
    
	Vec2i ptis[3];
	for (size_t i = 0; i < 3; i++) {
		ptis[i][0] = int(pts[i][0] + .5f);
		ptis[i][1] = int(pts[i][1] + .5f);
	}

	Vec2f bboxmin(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    Vec2f bboxmax(0, 0);
    Vec2f clamp(this->mImage->get_width() - 1, this->mImage->get_height() - 1);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 2; j++) {
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }

	Vec3f p;
	for (p[0] = int(bboxmin.x + .5f); p[0] <= bboxmax.x; p[0]++) {
		for (p[1] = int(bboxmin.y + .5f); p[1] <= bboxmax.y; p[1]++) {
			Vec3f bc_screen = barycentric<3>(pts[0], pts[1], pts[2], p);
			if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
            
            float pz = pts[0][2] * bc_screen[0] + pts[1][2] * bc_screen[1] + pts[2][2] * bc_screen[2];
            
            if (mZBuffer[ int(p.x + p.y * mImage->get_width()) ] < pz) continue;

            
            mZBuffer[ int(p.x + p.y * mImage->get_width()) ] = pz;
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
