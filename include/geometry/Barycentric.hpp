//
// Created by TangRuiwen on 2020/7/9.
//

#ifndef TINYRENDER_INCLUDE_GEOMETRY_BARYCENTRIC_HPP_
#define TINYRENDER_INCLUDE_GEOMETRY_BARYCENTRIC_HPP_
#include "glm/glm.hpp"

namespace TRM {

template<int DIMS, typename T>
glm::vec<3, float> barycentric(const glm::vec<DIMS, T> &a,
                               const glm::vec<DIMS, T> &b,
                               const glm::vec<DIMS, T> &c,
                               const glm::vec<DIMS, T> &p) {
  typedef glm::vec<3, float> return_type;
  return_type res;
  if (DIMS < 2) {
    return return_type{1.f, 1.f, 1.f};
  } else {
    return_type s[2];
    for (int i = 0; i < 2; i++) {
      s[i][0] = c[i] - a[i];
      s[i][1] = b[i] - a[i];
      s[i][2] = a[i] - p[i];
    }

    return_type u = glm::cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2) {
      return return_type(1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]);
    } else {
      return return_type{-1, 1, 1};
    }
  }
}

}

#endif //TINYRENDER_INCLUDE_GEOMETRY_BARYCENTRIC_HPP_
