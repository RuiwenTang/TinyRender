#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>

#include "LinkedList.h"

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

struct Object {
  virtual ~Object() = default;
};

struct Edge;

struct Vertex : public Object {
  // list
  Vertex* prev = nullptr;
  Vertex* next = nullptr;

  /**
   * All edge above and end with this vertex
   *
   *          head           tail
   *              \   ...    /
   *                   v
   *
   */
  LinkedList<Edge> edge_above = {};
  /**
   *  All edge below this vertex
   *
   *
   *              v
   *             / \
   *          head  tail
   *          /      \
   *            ...
   *
   */
  LinkedList<Edge> edge_below = {};

  // left enclosing edge during sweep line
  Edge* left = nullptr;
  // right enclosing edge during sweep line
  Edge* right = nullptr;

  glm::vec2 point = {};

  Vertex() = default;
  Vertex(glm::vec2 const& p) : point(p) {}

  ~Vertex() override = default;

  bool is_connected() const { return edge_above.head || edge_below.head; }

  void insert_above(Edge* e);
  void insert_below(Edge* e);
};

struct VertexCompare {
  inline bool operator()(Vertex* v1, Vertex* v2) {
    return Compare(v1->point, v2->point);
  }

  static bool Compare(glm::vec2 const& a, glm::vec2 const& b);
};

struct VertexList : public LinkedList<Vertex> {
  VertexList() = default;
  VertexList(Vertex* head, Vertex* tail) : LinkedList(head, tail) {}

  void insert(Vertex* v, Vertex* prev, Vertex* next);
  void remove(Vertex* v);

  void append(VertexList const& other);
  void append(Vertex* v);
  void prepend(Vertex* v);

  void close();
};

struct Polygon;

struct Edge : public Object {
  Vertex* top = nullptr;
  Vertex* bottom = nullptr;

  Edge* above_prev = nullptr;
  Edge* above_next = nullptr;
  Edge* below_prev = nullptr;
  Edge* below_next = nullptr;

  // left edge in active list during sweep line
  Edge* left = nullptr;
  // right edge in active list during sweep line
  Edge* right = nullptr;

  Edge* right_poly_prev = nullptr;
  Edge* right_poly_next = nullptr;
  Edge* left_poly_prev = nullptr;
  Edge* left_poly_next = nullptr;

  // left polygon during sweep line
  Polygon* left_poly = nullptr;
  // right polygon during sweep line
  Polygon* right_poly = nullptr;

  bool used_in_left = false;
  bool used_in_right = false;

  int32_t winding = 1;

  Edge(Vertex* top, Vertex* bottom, int32_t winding);

  ~Edge() override = default;

  // https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line
  // return > 0 means point in left
  // return < 0 means point in right
  double side_dist(glm::vec2 const& p);

  bool is_right_of(glm::vec2 const& p) { return side_dist(p) < 0.0; }

  bool is_left_of(glm::vec2 const& p) { return side_dist(p) > 0.0; }

  // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
  bool intersect(Edge* other, glm::vec2* point);

  void recompute();

  void set_bottom(Vertex* v);

  void set_top(Vertex* v);

 private:
  double le_a;
  double le_b;
  double le_c;
};

enum class Side {
  kLeft,
  kRight,
};

class ObjectHeap;

struct MonotonePolygon : public Object {
  Side side;

  Edge* first = nullptr;
  Edge* last = nullptr;

  int32_t winding;

  MonotonePolygon* prev = nullptr;
  MonotonePolygon* next = nullptr;

  MonotonePolygon(Edge* edge, Side side, int32_t winding)
      : side(side), winding(winding) {
    add_edge(edge);
  }

  ~MonotonePolygon() override = default;

  void add_edge(Edge* edge);
};

enum class FillType {
  kWinding,
  kEvenOdd,
};

struct Polygon : public Object {
  Vertex* first_vert;
  int32_t winding;

  // vertex count
  int32_t count = 0;
  Polygon* parent = nullptr;
  Polygon* next = nullptr;

  MonotonePolygon* head = nullptr;
  MonotonePolygon* tail = nullptr;

  Polygon(Vertex* first, int32_t winding)
      : first_vert(first), winding(winding) {}

  ~Polygon() override = default;

  Polygon* add_edge(Edge* e, Side side, ObjectHeap* heap);

  Vertex* last_vertex() const;
};

struct ActiveEdgeList : public LinkedList<Edge> {
  ActiveEdgeList() = default;
  ~ActiveEdgeList() = default;

  void insert(Edge* edge, Edge* prev, Edge* next);
  void insert(Edge* edge, Edge* prev);

  void append(Edge* edge);
  void remove(Edge* edge);

  bool contains(Edge* edge);

  // move event point from current to dst
  void rewind(Vertex** current, Vertex* dst);

  void find_enclosing(Vertex* v, Edge** left, Edge** right);
};

class ObjectHeap {
 public:
  ObjectHeap() = default;
  ~ObjectHeap() = default;

  template <class T, class... Args>
  T* Allocate(Args&&... args) {
    objs_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));

    return static_cast<T*>(objs_.back().get());
  }

 private:
  std::vector<std::unique_ptr<Object>> objs_ = {};
};

}  // namespace TRM
