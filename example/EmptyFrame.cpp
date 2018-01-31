#include <TGAImage.h>


static TRM::TGAColor WHITE(255, 255, 255);
static TRM::TGAColor RED(255, 0, 0);

int main(int argc, const char** argv) {
	
	TRM::TGAImage image(800, 600, TRM::TGAImage::RGB);

	image.clearColor(RED);

	image.write_tga_file("framebuffer.tga");
	return 0;
}