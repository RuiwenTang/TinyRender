#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace TRM {

struct Edge {
  glm::vec2 start;
  glm::vec2 end;
  int32_t winding = 1;

  Edge(glm::vec2 p1, glm::vec2 p2) : start(p1), end(p2) {
    if (start.x > end.x || (start.x == end.x && start.y > end.y)) {
      std::swap(start, end);
      winding = -1;
    }
  }
};

class EdgeList {
 public:
  EdgeList() = default;
  ~EdgeList() = default;

  bool add_edge(Edge const& edge);

  void sort_edge_list();

  std::vector<Edge*> sorted_list();

  std::vector<Edge> const& raw_list() { return m_edges; }

  const Edge* left_most() const;

  const Edge* right_most() const;

  int32_t left_most_index() const { return m_min_x_index; }

  int32_t right_most_index() const { return m_max_x_index; }

 private:
  // contains raw edge data
  // next.start == current.end
  std::vector<Edge> m_edges = {};
  std::vector<Edge*> m_sorted_edges = {};
  int32_t m_min_x_index = -1;
  int32_t m_max_x_index = -1;
};

}  // namespace TRM