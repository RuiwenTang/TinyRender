#include <Geometry.h>

namespace TRM {

template <>
template <>
vec<3, int>::vec(const vec<3, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}
template <>
template <>
vec<3, float>::vec(const vec<3, int> &v) : x(v.x), y(v.y), z(v.z) {}
template <>
template <>
vec<2, int>::vec(const vec<2, float> &v)
    : x(int(v.x + .5f)), y(int(v.y + .5f)) {}
template <>
template <>
vec<2, float>::vec(const vec<2, int> &v) : x(v.x), y(v.y) {}

Matrix lookAt(Vec3f eye, Vec3f center, Vec3f up) {
  Matrix ret;

  Vec3f f = (center - eye).normalize();  // z axis
  Vec3f s = cross(f, up).normalize();    // x axis
  Vec3f u = cross(s, f).normalize();     // y axis

  ret[0][0] = s[0];
  ret[0][1] = s[1];
  ret[0][2] = s[2];
  ret[1][0] = u[0];
  ret[1][1] = u[1];
  ret[1][2] = u[2];
  ret[2][0] = -f[0];
  ret[2][1] = -f[1];
  ret[2][2] = -f[2];
  ret[0][3] = -1.f * (s * eye);
  ret[1][3] = -1.f * (u * eye);
  ret[2][3] = (f * eye);
  ret[3][3] = 1.f;
  return ret;
}

Matrix perspective(float fovy, float aspect, float near, float far) {
  Matrix ret;

  ret.identity();

  ret[0][0] = 1.f / (aspect * std::tanf(fovy / 2.f));
  ret[1][1] = 1.f / std::tanf(fovy / 2.f);
  ret[2][2] = -1.f * (far + near) / (far - near);
  ret[2][3] = -2.f * (far * near) / (far - near);

  ret[3][2] = -1.f;
  ret[3][3] = 0.f;

  return ret;
}

float degToRadian(float deg) { return deg * PI / 180.f; }

}  // namespace TRM