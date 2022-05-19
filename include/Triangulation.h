#pragma once

#include <functional>
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

  Edge(glm::vec2 p1, glm::vec2 p2, bool) : start(p1), end(p2) {}
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

class Triangulation {
 public:
  Triangulation(std::vector<Edge> upper_chain, std::vector<Edge> lower_chain);
  ~Triangulation() = default;

  void do_triangulate(std::function<void(glm::vec2 const&, glm::vec2 const&,
                                         glm::vec2 const&)> const& callback);

 private:
  void init_state();
  bool next_is_upper();
  void push_and_move(bool upper);
  glm::vec2 next_point(bool upper);
  float calculate_k(glm::vec2 const& p1, glm::vec2 const& p2, bool upper);

  bool check_visible(float prev_k, float current_k, bool upper);

 private:
  std::vector<Edge> m_upper_chain;
  std::vector<Edge> m_lower_chain;
  std::vector<glm::vec2> m_point_stack = {};
  // curent vi-1 is upper chain
  bool m_upper = true;
  size_t m_current_upper_index = 0;
  size_t m_current_lower_index = 0;
};

}  // namespace TRM