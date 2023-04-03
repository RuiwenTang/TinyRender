#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "LinkedList.h"

namespace TRM {

struct Object {
  virtual ~Object() = default;
};

struct Edge;

struct Vertex : public Object {
  // list
  Vertex* prev = nullptr;
  Vertex* next = nullptr;

  // all edge above this point, sorted by y first, then x
  LinkedList<Edge> edge_above = {};
  LinkedList<Edge> edge_below = {};

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

struct Edge : public Object {
  Vertex* top = nullptr;
  Vertex* bottom = nullptr;

  Edge* above_prev = nullptr;
  Edge* above_next = nullptr;
  Edge* below_prev = nullptr;
  Edge* below_next = nullptr;

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

class Triangulation {
 public:
  Triangulation();
  ~Triangulation() = default;

  void add_path(std::vector<glm::vec2> const& points);

  void do_triangulate(std::function<void(glm::vec2 const&, glm::vec2 const&,
                                         glm::vec2 const&)> const& callback);

 private:
  void build_mesh();

  void merge_vertices();

  Edge* make_edge(Vertex* p1, Vertex* p2);

 private:
  ObjectHeap heap_;
  std::vector<VertexList> out_lines_;
  VertexList mesh_;
};

}  // namespace TRM
