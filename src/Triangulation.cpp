#include <Triangulation.h>

#include <algorithm>
#include <limits>

namespace TRM {

void Vertex::insert_above(Edge* e) {
  if (e->top->point == e->bottom->point ||  // horizontal
      VertexCompare::Compare(e->bottom->point, e->top->point)) {
    return;
  }

  Edge* above_prev = nullptr;
  Edge* above_next = nullptr;

  for (above_next = edge_above.head; above_next != nullptr;
       above_next = above_next->above_next) {
    if (above_next->is_right_of(e->top->point)) {
      break;
    }

    above_prev = above_next;
  }

  LinkedList<Edge>::Insert<&Edge::above_prev, &Edge::above_next>(
      e, above_prev, above_next, &edge_above.head, &edge_above.tail);
}

void Vertex::insert_below(Edge* e) {
  if (e->top->point == e->bottom->point ||
      VertexCompare::Compare(e->bottom->point, e->top->point)) {
    return;
  }

  Edge* below_prev = nullptr;
  Edge* below_next = nullptr;

  for (below_next = edge_below.head; below_next != nullptr;
       below_next = below_next->below_next) {
    if (below_next->is_right_of(e->bottom->point)) {
      break;
    }

    below_prev = below_next;
  }

  LinkedList<Edge>::Insert<&Edge::below_prev, &Edge::below_next>(
      e, below_prev, below_next, &edge_below.head, &edge_below.tail);
}

bool VertexCompare::Compare(const glm::vec2& a, const glm::vec2& b) {
  return a.y < b.y || (a.y == b.y && a.x < b.x);
}

void VertexList::insert(Vertex* v, Vertex* prev, Vertex* next) {
  Insert<&Vertex::prev, &Vertex::next>(v, prev, next, &head, &tail);
}

void VertexList::remove(Vertex* v) {
  Remove<&Vertex::prev, &Vertex::next>(v, &head, &tail);
}

void VertexList::append(VertexList const& other) {
  if (other.head == nullptr) {
    return;
  }

  if (tail) {
    tail->next = other.head;
    other.head->prev = tail;
  } else {
    head = other.head;
  }

  tail = other.tail;
}

void VertexList::append(Vertex* v) { insert(v, tail, nullptr); }

void VertexList::prepend(Vertex* v) { insert(v, nullptr, head); }

void VertexList::close() {
  if (head && tail) {
    tail->next = head;
    head->prev = tail;
  }
}

Edge::Edge(Vertex* top, Vertex* bottom, int32_t winding)
    : top(top),
      bottom(bottom),
      winding(winding),
      le_a(static_cast<double>(bottom->point.y) - top->point.y),
      le_b(static_cast<double>(top->point.x) - bottom->point.x),
      le_c(static_cast<double>(top->point.y) * bottom->point.x -
           static_cast<double>(top->point.x) * bottom->point.y) {}

double Edge::side_dist(const glm::vec2& p) {
  return le_a * p.x + le_b * p.y + le_c;
}

bool Edge::intersect(Edge* other, glm::vec2* point) {
  if (top == other->top || bottom == other->bottom || top == other->bottom ||
      bottom == other->top) {
    return false;
  }

  // check if two aabb bounds is intersect
  if (std::min(top->point.x, bottom->point.x) >
          std::max(other->top->point.x, other->bottom->point.x) ||
      std::max(top->point.x, bottom->point.x) <
          std::min(other->top->point.x, other->bottom->point.x) ||
      std::min(top->point.y, bottom->point.y) >
          std::max(other->top->point.y, other->bottom->point.y) ||
      std::max(top->point.y, bottom->point.y) <
          std::min(other->top->point.y, other->bottom->point.y)) {
    return false;
  }

  double denom = le_a * other->le_b - le_b * other->le_a;

  if (denom == 0.0) {
    return false;
  }

  double scale = 1.0 / denom;

  point->x = glm::round(
      static_cast<float>((le_b * other->le_c - other->le_b * le_c) * scale));
  point->y = glm::round(
      static_cast<float>((other->le_a * le_c - le_a * other->le_c) * scale));

  if (glm::isinf(point->x) || glm::isinf(point->y)) {
    return false;
  }

  return true;
}

void Edge::recompute() {
  le_a = static_cast<double>(bottom->point.y) - top->point.y;
  le_b = static_cast<double>(top->point.x) - bottom->point.x;
  le_c = static_cast<double>(top->point.y) * bottom->point.x -
         static_cast<double>(top->point.x) * bottom->point.y;
}

void Edge::set_top(Vertex* v) {
  // remove this edge from top's below list
  LinkedList<Edge>::Remove<&Edge::below_prev, &Edge::below_next>(
      this, &top->edge_below.head, &top->edge_below.tail);
  // update top vertex
  top = v;
  // recompute line equation
  recompute();
  // insert self to new top's below list
  top->insert_below(this);
}

void Edge::set_bottom(Vertex* v) {
  // remove this edge from bottom's above list
  LinkedList<Edge>::Remove<&Edge::above_prev, &Edge::above_next>(
      this, &bottom->edge_above.head, &bottom->edge_above.tail);
  // update bottom vertex
  bottom = v;
  // recompute line equation
  recompute();
  // insert self to new bottom's above list
  bottom->insert_below(this);
}

void ActiveEdgeList::insert(Edge* edge, Edge* prev, Edge* next) {
  LinkedList<Edge>::Insert<&Edge::left, &Edge::right>(edge, prev, next, &head,
                                                      &tail);
}

void ActiveEdgeList::insert(Edge* edge, Edge* prev) {
  auto next = prev ? prev->right : head;

  insert(edge, prev, next);
}

void ActiveEdgeList::append(Edge* edge) { insert(edge, tail, nullptr); }

void ActiveEdgeList::remove(Edge* edge) {
  LinkedList<Edge>::Remove<&Edge::left, &Edge::right>(edge, &head, &tail);
}

bool ActiveEdgeList::contains(Edge* edge) {
  return edge->left || edge->right || head == edge;
}

void ActiveEdgeList::rewind(Vertex** current, Vertex* dst) {
  if (!current || *current == dst ||
      VertexCompare::Compare((*current)->point, dst->point)) {
    return;
  }

  Vertex* v = *current;

  while (v != dst) {
    v = v->prev;

    for (auto e = v->edge_below.head; e; e = e->below_next) {
      this->remove(e);
    }

    auto left = v->left;

    for (auto e = v->edge_above.head; e; e = e->above_next) {
      this->insert(e, left);
      left = e;

      auto top = e->top;
      if (VertexCompare::Compare(top->point, dst->point) &&
          ((top->left && !top->left->is_left_of(e->top->point)) ||
           (top->right && !top->right->is_right_of(e->top->point)))) {
        dst = top;
      }
    }
  }

  *current = v;
}

void ActiveEdgeList::find_enclosing(Vertex* v, Edge** left, Edge** right) {
  if (v->edge_above.head && v->edge_above.tail) {
    *left = v->edge_above.head->left;
    *right = v->edge_above.tail->right;
    return;
  }

  Edge* prev = nullptr;
  Edge* next = nullptr;

  // walk through aet to get left most edge
  for (prev = tail; prev != nullptr; prev = prev->left) {
    if (prev->is_left_of(v->point)) {
      break;
    }
    next = prev;
  }

  *left = prev;
  *right = next;
}

Triangulation::Triangulation() : heap_(), out_lines_(), mesh_() {}

void Triangulation::add_path(const std::vector<glm::vec2>& points) {
  out_lines_.emplace_back(VertexList{});

  for (auto const& p : points) {
    auto v = heap_.Allocate<Vertex>(p);

    out_lines_.back().append(v);
  }
}

void Triangulation::do_triangulate(
    const std::function<void(const glm::vec2&, const glm::vec2&,
                             const glm::vec2&)>& callback) {
  build_mesh();

  merge_vertices();

  flat_mesh();
}

void Triangulation::build_mesh() {
  std::vector<Vertex*> temp{};

  for (auto const& list : out_lines_) {
    auto prev = list.tail;
    auto v = list.head;

    while (v != nullptr) {
      auto next = v->next;

      auto edge = make_edge(prev, v);

      if (edge) {
        edge->bottom->insert_above(edge);
        edge->top->insert_below(edge);
      }

      temp.emplace_back(v);

      prev = v;
      v = next;
    }
  }

  std::sort(temp.begin(), temp.end(), VertexCompare{});

  for (auto const& v : temp) {
    mesh_.append(v);
  }
}

void Triangulation::merge_vertices() {
  if (!mesh_.head) {
    // mesh is empty
    return;
  }

  for (auto v = mesh_.head->next; v;) {
    auto next = v->next;

    if (VertexCompare::Compare(v->point, v->prev->point)) {
      // already sorted, if means this two points is same
      v->point = v->prev->point;
    }

    if (v->point == v->prev->point) {
      // merge v into v->prev
      while (auto e = v->edge_above.head) {
        e->set_bottom(v->prev);
      }

      while (auto e = v->edge_below.head) {
        e->set_top(v->prev);
      }

      mesh_.remove(v);
    }

    v = next;
  }
}

Edge* Triangulation::make_edge(Vertex* t, Vertex* b) {
  if (!t || !b || t->point == b->point) {
    return nullptr;
  }

  int32_t winding = 1;

  if (VertexCompare::Compare(b->point, t->point)) {
    winding = -1;
    std::swap(t, b);
  }

  return heap_.Allocate<Edge>(t, b, winding);
}

bool Triangulation::check_intersection(Edge* left, Edge* right,
                                       ActiveEdgeList* ael, Vertex** current) {
  if (!left || !right) {
    return false;
  }

  glm::vec2 p;

  if (left->intersect(right, &p) && !glm::isinf(p.x) && !glm::isinf(p.y)) {
    Vertex* v;
    Vertex* top = *current;

    // the vertex in mesh is sorted, so walk to prev can find latest top point
    while (top && VertexCompare::Compare(p, top->point)) {
      top = top->prev;
    }

    if (p == left->top->point) {
      v = left->top;
    } else if (p == left->bottom->point) {
      v = left->bottom;
    } else if (p == right->top->point) {
      v = right->top;
    } else if (p == right->bottom->point) {
      v = right->bottom;
    } else {
      // intersect point is between start and end point
      // need to insert new vertex
      auto prev = top;
      while (prev && VertexCompare::Compare(p, prev->point)) {
        prev = prev->prev;
      }

      auto next = prev ? prev->next : mesh_.head;
      while (next && VertexCompare::Compare(next->point, p)) {
        prev = next;
        next = next->next;
      }

      // check if point is already in mesh
      if (prev && prev->point == p) {
        v = prev;
      } else if (next && next->point == p) {
        v = next;
      } else {
        v = heap_.Allocate<Vertex>(p);
        v->point = p;

        mesh_.insert(v, prev, next);
      }

      ael->rewind(current, top ? top : v);

      split_edge(left, v, ael, current);
      split_edge(right, v, ael, current);
    }
    return true;
  }

  return intersect_pair_edge(left, right, ael, current);
}

bool Triangulation::split_edge(Edge* edge, Vertex* v, ActiveEdgeList* ael,
                               Vertex** current) {
  if (!edge->top || !edge->bottom || v == edge->top || v == edge->bottom) {
    return false;
  }

  int32_t winding = edge->winding;

  Vertex* top;
  Vertex* bottom;

  if (VertexCompare::Compare(v->point, edge->top->point)) {
    /**
     *
     *   v
     *    \
     *     \
     *     top
     *      \
     *       \
     *        \
     *       bottom
     */
    top = v;
    bottom = edge->top;
    winding *= -1;

    edge->set_top(v);
  } else if (VertexCompare::Compare(edge->bottom->point, v->point)) {
    /**
     *
     *   top
     *    \
     *     \
     *    bottom
     *      \
     *       \
     *        \
     *         v
     */
    top = edge->bottom;
    bottom = v;
    winding *= -1;

    edge->set_bottom(v);
  } else {
    /**
     *
     *   top
     *    \
     *     \
     *      v
     *      \
     *       \
     *        \
     *       bottom
     */
    top = v;
    bottom = edge->bottom;
    edge->set_bottom(v);
  }

  auto new_edge = heap_.Allocate<Edge>(top, bottom, winding);

  bottom->insert_above(new_edge);
  top->insert_below(new_edge);

  return true;
}

bool Triangulation::intersect_pair_edge(Edge* left, Edge* right,
                                        ActiveEdgeList* ael, Vertex** current) {
  if (!left->top || !left->bottom || !right->top || !right->bottom) {
    return false;
  }

  if (left->top == right->top || left->bottom == right->bottom) {
    return false;
  }

  Edge* split = nullptr;

  Vertex* split_at = nullptr;

  // check if these two edge is intersected
  if (VertexCompare::Compare(left->top->point, right->top->point)) {
    if (!left->is_left_of(right->top->point)) {
      split = left;
      split_at = right->top;
    }
  } else {
    if (!right->is_right_of(left->top->point)) {
      split = right;
      split_at = left->top;
    }
  }

  if (VertexCompare::Compare(right->bottom->point, left->bottom->point)) {
    if (!left->is_left_of(right->bottom->point)) {
      split = left;
      split_at = right->bottom;
    }
  } else {
    if (!right->is_right_of(left->bottom->point)) {
      split = right;
      split_at = left->bottom;
    }
  }

  if (!split) {
    return false;
  }

  ael->rewind(current, split->top);

  return split_edge(split, split_at, ael, current);
}

void Triangulation::flat_mesh() {
  ActiveEdgeList ael;

  for (Vertex* v = mesh_.head; v != nullptr; v = v->next) {
    if (v->edge_above.head == nullptr && v->edge_below.head == nullptr) {
      continue;
    }

    Edge* left_enclosing = nullptr;
    Edge* right_enclosing = nullptr;

    bool intersected = false;

    do {
      intersected = false;

      ael.find_enclosing(v, &left_enclosing, &right_enclosing);

      v->left = left_enclosing;
      v->right = right_enclosing;

      if (v->edge_below.head) {
        for (auto e = v->edge_below.head; e; e = e->below_next) {
          // check current edge is intersected by left or right neighbor edges
          if (check_intersection(left_enclosing, e, &ael, &v) ||
              check_intersection(e, right_enclosing, &ael, &v)) {
            intersected = true;
            // find intersection between current and it's neighbor
            break;
          }
        }
      } else {
        // check left and right intersection
        if (check_intersection(left_enclosing, right_enclosing, &ael, &v)) {
          intersected = true;
        }
      }

    } while (intersected);

    // we are done for all edge end with current point
    for (auto e = v->edge_above.head; e; e = e->above_next) {
      ael.remove(e);
    }

    auto left = left_enclosing;

    // insert all edge start from current point into ael
    for (auto e = v->edge_below.head; e; e = e->below_next) {
      ael.insert(e, left);
      left = e;
    }
  }
}

}  // namespace TRM
