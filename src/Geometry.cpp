#include <Geometry.h>

namespace TRM {

template <> template <> vec<3,int>  ::vec(const vec<3,float> &v) : x(int(v.x+.5f)),y(int(v.y+.5f)),z(int(v.z+.5f)) {}
template <> template <> vec<3,float>::vec(const vec<3,int> &v)   : x(v.x),y(v.y),z(v.z) {}
template <> template <> vec<2,int>  ::vec(const vec<2,float> &v) : x(int(v.x+.5f)),y(int(v.y+.5f)) {}
template <> template <> vec<2,float>::vec(const vec<2,int> &v)   : x(v.x),y(v.y) {}

Matrix perspective(float fovy, float aspect, float near, float far) {
    Matrix ret;

    ret[0][0] = 1.f / (aspect * std::tanf(fovy / 2.f));
    ret[1][1] = 1.f / std::tanf(fovy / 2.f);
    ret[2][2] = -1.f * (far + near) / (far - near);
    ret[2][3] = -2.f * (far * near) / (far - near);

    ret[3][2] = -1;
    ret[3][3] = 0;

    return ret;
}

}