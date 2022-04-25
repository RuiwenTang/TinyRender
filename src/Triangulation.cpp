#include <Triangulation.h>

#include <algorithm>
#include <limits>

namespace TRM {

struct EdgeCompare {
  bool operator()(Edge const& e1, Edge const& e2) {
    auto p1d = e1.start - e2.start;
    auto p2d = e1.end - e2.end;

    if (p1d.x < 0.f) {
      return true;
    }

    if (p1d.x == 0.f && p1d.y < 0.f) {
      return true;
    }

    if (p2d.x < 0.f) {
      return true;
    }

    if (p2d.x == 0.f && p2d.y < 0.f) {
      return true;
    }

    return false;
  }
};

bool EdgeList::add_edge(const Edge& edge) {
  m_edges.emplace_back(edge);

  return true;
}

void EdgeList::sort_edge_list() {
  std::sort(m_edges.begin(), m_edges.end(), EdgeCompare{});
}

const Edge* EdgeList::left_most() const {
  if (m_min_x_index == -1) {
    return nullptr;
  }

  return &m_edges[m_min_x_index];
}

const Edge* EdgeList::right_most() const {
  if (m_max_x_index == -1) {
    return nullptr;
  }

  return &m_edges[m_max_x_index];
}

}  // namespace TRM