
#include <TGAImage.h>
#include <TinyRender.h>
#include <Model.h>
#include <config.h>
#include <string>
#include <random>

using namespace TRM;

#define WIDTH   800
#define HEIGHT  600

static const TGAColor WHITE(255,255,255);

static const std::string ASSETS_ROOT = ASSETS_PATH;

int main(int argc, const char** argv) {
    
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    framebuffer.clearColor(TGAColor(0, 0, 0));

    TinyRender render;
    render.attachBuffer(&framebuffer);
    
    std::string model_path = ASSETS_ROOT + "/african_head.obj";

    
    Model model(model_path.c_str());
    
    Vec3f light_dir;
    light_dir[0] = 0;
    light_dir[1] = 0;
    light_dir[2] = -1.f;
    
    /** draw all vertex triangle ***/
    for (size_t i = 0; i < model.nfaces(); i++) {
        std::vector<int> face = model.face(i);
        Vec2i triangle[3];
        Vec3f vectors[3];
        for(size_t j = 0; j < 3; j++) {
            Vec3f c = model.vert(face[j]);
            vectors[j] = c;
            c[0] = (c[0] + 1.f) * WIDTH / 2.f;
            c[1] = (c[1] + 1.f) * HEIGHT / 2.f;
            triangle[j][0] = (int)c[0];
            triangle[j][1] = (int)c[1];
            
        }
        Vec3f normal = ((vectors[2] - vectors[0])^(vectors[1] - vectors[0])).normalize();
        
        float indensity = normal * light_dir;
        
         if (indensity > 0) {
            TGAColor color(255 * indensity, 255 * indensity, 255 * indensity);
            TGAColor colors[] = { color, color, color };
             render.triangle(triangle[0], triangle[1], triangle[2], color);
         }
        
    }
    
    framebuffer.flip_vertically();
    framebuffer.write_png_stb("framebuffer.png");
    
    return 0;
}
