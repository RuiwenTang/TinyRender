#include <Geometry.h>
#include <TGAImage.h>
#include <TinyRender.h>
#include <iostream>

using namespace TRM;

static TGAColor WHITE(255, 255, 255);
static TGAColor RED(255, 0, 0);


int main(int argc, const char** argv) {
    
	TGAImage image(800, 600, TGAImage::RGB);
	image.clearColor(WHITE);
	TinyRender render;
	render.attachBuffer(&image);

	Vec2i a, b, c;
	a[0] = 400; a[1] = 100;
	b[0] = 100; b[1] = 500;
	c[0] = 700; c[1] = 500;

	render.triangle(a, b, c, RED);

	image.write_tga_file("framebuffer.tga");

    return 0;
}
