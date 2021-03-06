#include <TGAImage.h>
#include <TinyRender.h>
#include <Model.h>
#include <config.h>
#include <string>

using namespace TRM;

#define WIDTH 800
#define HEIGHT 600

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {

    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    framebuffer.clearColor(TGAColor(0, 0, 0));

    TinyRender render;
    render.attachBuffer(&framebuffer);
    
    std::string model_path = ASSETS_ROOT + "/african_head.obj";
    std::string texture_path = ASSETS_ROOT + "/african_head_diffuse.tga";
    
    Model model(model_path.c_str());
    
    Vec3f light_dir;
    light_dir[0] = 0;
    light_dir[1] = 0;
    light_dir[2] = -1.f;

    TGAImage texture;
    texture.read_tga_file(texture_path.c_str());
    texture.flip_vertically();

    /** draw all vertex triangle ***/
    for (size_t i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        Vec3f triangle[3];
        Vec3f vectors[3];
        for(size_t j = 0; j < 3; j++) {
            Vec3f c = model.vert(face[j]);
            vectors[j] = c;
            c[0] = (c[0] + 1.f) * WIDTH / 2.f;
            c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
            triangle[j][0] = c[0];
            triangle[j][1] = c[1];
            triangle[j][2] = c[2];
        }
        Vec3f normal = ((vectors[2] - vectors[0])^(vectors[1] - vectors[0])).normalize();



        Vec2f uv[] = { model.uv(i, 0), model.uv(i, 1), model.uv(i, 2) };
        render.triangle(triangle, uv, &texture);
    }
    
    framebuffer.flip_vertically();
    framebuffer.write_png_stb("framebuffer.png");

    return 0;
}