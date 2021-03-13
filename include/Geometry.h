#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>


namespace TRM {

#define PI 3.1415926f

/////////////////////////////////////////////////////////////////////////////////

typedef glm::vec2 Vec2f;
typedef glm::ivec2 Vec2i;
typedef glm::vec3 Vec3f;
typedef glm::ivec3 Vec3i;
typedef glm::vec4 Vec4f;
typedef glm::mat4 Matrix;

///////////////////////////////////////////////////////////////////////////////////

template <size_t DimRows>
Vec3f barycentric(glm::vec<DimRows, float, glm::defaultp> A,
                  glm::vec<DimRows, float, glm::defaultp> B,
                  glm::vec<DimRows, float, glm::defaultp> C,
                  glm::vec<DimRows, float, glm::defaultp> P) {
  if (DimRows < 2) {
    return Vec3f(1., 1., 1.);
  } else {
    Vec3f s[2];
    for (int i = 0; i < 2; i++) {
      s[i][0] = C[i] - A[i];
      s[i][1] = B[i] - A[i];
      s[i][2] = A[i] - P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2)  // dont forget that u[2] is integer. If it is
                                // zero then triangle ABC is degenerate
      return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    return Vec3f(-1, 1, 1);  // in this case generate negative coordinates, it
                             // will be thrown away by the rasterizator
  }
}

Matrix lookAt(Vec3f eye, Vec3f look, Vec3f up);
Matrix perspective(float fovy, float aspect, float near, float far);

template <size_t DIM, typename T>
static inline glm::vec<DIM, T, glm::defaultp> Normalize(
    glm::vec<DIM, T, glm::defaultp> const& v) {
  return glm::normalize(v);
}

static inline Matrix IdentityMatrix() { return glm::identity<Matrix>(); }

float degToRadian(float deg);

}  // namespace TRM
