#include <TGAImage.h>
#include <TinyRender.h>
#include <Model.h>
#include <config.h>
#include <string>

using namespace TRM;

#define WIDTH   800
#define HEIGHT  600

static const TGAColor WHITE(255,255,255);

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {
    
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGBA);
    
    TinyRender render;
    render.attachBuffer(&framebuffer);
    framebuffer.clearColor(TGAColor{0, 0, 0});
    
    std::string model_path = ASSETS_ROOT + "/african_head.obj";
    
    
    Model model(model_path.c_str());
    /** draw all vertex triangle ***/
    for (size_t i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        Vec3f triangle[3];
        for(size_t j = 0; j < 3; j++) {
            Vec3f c = model.vert(face[j]);
            c[0] = (c[0] + 1.f) * WIDTH / 2.f;
            c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
            triangle[j] = c;
        }
        render.line(triangle[0][0], triangle[0][1], triangle[1][0], triangle[1][1], WHITE);
        render.line(triangle[1][0], triangle[1][1], triangle[2][0], triangle[2][1], WHITE);
        render.line(triangle[2][0], triangle[2][1], triangle[0][0], triangle[0][1], WHITE);
        
    }
    
    framebuffer.flip_vertically();
    framebuffer.write_png_stb("framebuffer.png");
    
    return 0;
}
