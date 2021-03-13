#include <Geometry.h>

namespace TRM {

Matrix lookAt(Vec3f eye, Vec3f center, Vec3f up) {
  return glm::lookAt(eye, center, up);
}

Matrix perspective(float fovy, float aspect, float near, float far) {
  return glm::perspective(fovy, aspect, near, far);
}

float degToRadian(float deg) { return deg * PI / 180.f; }

}  // namespace TRM