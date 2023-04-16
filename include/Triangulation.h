#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "Geometry.h"

namespace TRM {

class Triangulation {
 public:
  Triangulation();
  ~Triangulation() = default;

  void set_filltype(FillType type);

  void add_path(std::vector<glm::vec2> const& points);

  void do_triangulate(std::function<void(glm::vec2 const&, glm::vec2 const&,
                                         glm::vec2 const&)> const& callback);

 private:
  void build_mesh();

  void merge_vertices();

  void simplify_mesh();

  void tess_mesh();

  bool match_filltype(Polygon* poly);

  Edge* make_edge(Vertex* p1, Vertex* p2);

  bool check_intersection(Edge* left, Edge* right, ActiveEdgeList* ael,
                          Vertex** current);

  bool split_edge(Edge* edge, Vertex* v, ActiveEdgeList* ael, Vertex** current);

  bool intersect_pair_edge(Edge* left, Edge* right, ActiveEdgeList* ael,
                           Vertex** current);

  Polygon* make_poly(Vertex* v, int32_t winding);

  void emit_poly(MonotonePolygon* poly,
                 const std::function<void(const glm::vec2&, const glm::vec2&,
                                          const glm::vec2&)>& callback);
 private:
  ObjectHeap heap_;
  std::vector<VertexList> out_lines_;
  VertexList mesh_;
  FillType fill_type_ = FillType::kWinding;
  Polygon* poly_list_ = nullptr;
};

}  // namespace TRM
