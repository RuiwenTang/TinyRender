#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <limits>
#include <memory>
#include <vector>

#include "Geometry.h"

namespace TRM {

struct Rect {
  float left;
  float top;
  float right;
  float bottom;

  Rect()
      : left(std::numeric_limits<float>::max()),
        top(std::numeric_limits<float>::max()),
        right(std::numeric_limits<float>::min()),
        bottom(std::numeric_limits<float>::min()) {}

  bool contains(Rect const& r);

  bool intersects(Rect const& r);

  bool is_empty() const;
};

class PolygonMerge {
 public:
  using Path = std::vector<glm::vec2>;

  PolygonMerge();
  ~PolygonMerge() = default;

  void set_filltype(FillType type);

  bool do_merge(std::vector<glm::vec2> const& p1,
                std::vector<glm::vec2> const& p2);

 private:
  void add_polygon(std::vector<glm::vec2> const& poly, Rect* bounds);

  void build_mesh();

  void merge_vertices();

  void simplify_mesh();

  void merge_mesh();

  Edge* make_edge(Vertex* p1, Vertex* p2);

  bool check_intersection(Edge* left, Edge* right, ActiveEdgeList* ael,
                          Vertex** current);

  bool split_edge(Edge* edge, Vertex* v, ActiveEdgeList* ael, Vertex** current);

  bool intersect_pair_edge(Edge* left, Edge* right, ActiveEdgeList* ael,
                           Vertex** current);

  void remove_inner_edges();

  void extract_boundary(Edge* edge);

  bool match_filltype(int32_t winding);

  void begin_path();

 private:
  ObjectHeap heap_;
  std::vector<VertexList> out_lines_;
  VertexList mesh_;
  FillType fill_type_ = FillType::kWinding;
  Rect rect_1_ = {};
  Rect rect_2_ = {};
  std::vector<Path> result_ = {};
};

}  // namespace TRM
