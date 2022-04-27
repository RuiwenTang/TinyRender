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

Triangulation::Triangulation(std::vector<Edge> upper_chain,
                             std::vector<Edge> lower_chain)
    : m_upper_chain(std::move(upper_chain)),
      m_lower_chain(std::move(lower_chain)) {}

void Triangulation::do_triangulate(
    const std::function<void(const glm::vec2&, const glm::vec2&,
                             const glm::vec2&)>& callback) {
  if (m_lower_chain.empty() || m_upper_chain.empty()) {
    // some thing is wrong
    return;
  }

  if (m_lower_chain.front().start != m_upper_chain.front().start) {
    return;
  }

  this->init_state();

  while (m_current_lower_index < m_lower_chain.size() &&
         m_current_upper_index < m_upper_chain.size()) {
    bool next_upper = next_is_upper();

    if (m_upper == next_upper) {  // next point is on same chain
      if (m_point_stack.size() <= 1) {
        push_and_move(m_upper);
        continue;
      }

      glm::vec2 p = next_point(m_upper);

      float prev_k =
          calculate_k(m_point_stack[m_point_stack.size() - 2],
                      m_point_stack[m_point_stack.size() - 1], m_upper);

      float current_k =
          calculate_k(m_point_stack[m_point_stack.size() - 1], p, m_upper);

      if (!check_visible(prev_k, current_k, m_upper)) {
        push_and_move(m_upper);
        continue;
      }

      do {
        // notify callback
        callback(p, m_point_stack[m_point_stack.size() - 1],
                 m_point_stack[m_point_stack.size() - 2]);

        m_point_stack.pop_back();

        if (m_point_stack.size() < 2) {
          break;
        }

        float prev_k =
            calculate_k(m_point_stack[m_point_stack.size() - 2],
                        m_point_stack[m_point_stack.size() - 1], m_upper);

        float current_k =
            calculate_k(m_point_stack[m_point_stack.size() - 1], p, m_upper);

        if (!check_visible(prev_k, current_k, m_upper)) {
          break;
        }

      } while (m_point_stack.size() >= 2);

      push_and_move(m_upper);
    } else {  // next point is on reflex chain
      glm::vec2 p = next_point(next_upper);

      // triangle all remains point on stack
      while (m_point_stack.size() > 1) {
        callback(p, m_point_stack[0], m_point_stack[1]);

        m_point_stack.erase(m_point_stack.begin());
      }

      push_and_move(next_upper);
      m_upper = next_upper;
    }
  }
}

void Triangulation::init_state() {
  // at begining both upper-chain and lower-chain should start at same point
  m_point_stack.emplace_back(m_upper_chain.front().start);

  // push next active point to stack and indicat if it on lower chain
  if (m_upper_chain.front().end.x > m_lower_chain.front().end.x) {
    m_upper = false;
    m_current_lower_index++;
    m_point_stack.emplace_back(m_lower_chain.front().end);
  } else {
    m_current_upper_index++;
    m_point_stack.emplace_back(m_upper_chain.front().end);
  }
}

bool Triangulation::next_is_upper() {
  return m_upper_chain[m_current_upper_index].end.x <=
         m_lower_chain[m_current_lower_index].end.x;
}

void Triangulation::push_and_move(bool upper) {
  glm::vec2 p = next_point(upper);

  m_point_stack.emplace_back(p);

  if (upper) {
    m_current_upper_index++;
  } else {
    m_current_lower_index++;
  }
}

glm::vec2 Triangulation::next_point(bool upper) {
  if (upper) {
    return m_upper_chain[m_current_upper_index].end;
  } else {
    return m_lower_chain[m_current_lower_index].end;
  }
}

float Triangulation::calculate_k(glm::vec2 const& p1, glm::vec2 const& p2,
                                 bool upper) {
  if (p1.x == p2.x) {
    return upper ? -99999.f : 99999.f;
  }

  return (p2.y - p1.y) / (p2.x - p1.x);
}

bool Triangulation::check_visible(float prev_k, float current_k, bool upper) {
  if (upper) {
    return current_k > prev_k;
  } else {
    return current_k < prev_k;
  }
}

}  // namespace TRM