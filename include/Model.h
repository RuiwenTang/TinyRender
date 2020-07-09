#pragma once

#include <vector>
#include <string>
#include <Geometry.h>
#include <TGAImage.h>

namespace TRM {

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i>> faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    TGAImage normalmap_;
    TGAImage specularmap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f normal(int iface, int nthvert);
    Vec3f normal(Vec2f uv);
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    Vec2f uv(int iface, int nthvert);
    Color diffuse(Vec2f uv);
    float specular(Vec2f uv);
    std::vector<int> face(int idx);
};

}