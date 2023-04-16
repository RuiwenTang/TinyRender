#include <PolygonMerge.h>

#include <glm/gtx/string_cast.hpp>

namespace TRM {

bool Rect::contains(Rect const& r) {
  return !this->is_empty() && left <= r.left && top <= r.top &&
         right >= r.right && bottom >= r.bottom;
}

bool Rect::intersects(Rect const& r) {
  float L = std::max(left, r.left);
  float R = std::min(right, r.right);
  float T = std::max(top, r.top);
  float B = std::min(bottom, r.bottom);

  return L < R && T < B;
}

bool Rect::is_empty() const {
  float width = right - left;
  float height = bottom - top;

  if (width <= 0.f || height <= 0.f) {
    return true;
  }

  return false;
}

PolygonMerge::PolygonMerge() : heap_(), out_lines_(), mesh_() {}

void PolygonMerge::set_filltype(FillType type) { fill_type_ = type; }

void PolygonMerge::add_polygon(const std::vector<glm::vec2>& points,
                               Rect* bounds) {
  out_lines_.emplace_back(VertexList{});

  for (auto const& p : points) {
    auto v = heap_.Allocate<Vertex>(p);

    bounds->left = std::min(bounds->left, p.x);
    bounds->top = std::min(bounds->top, p.y);
    bounds->right = std::max(bounds->right, p.x);
    bounds->bottom = std::max(bounds->bottom, p.y);

    out_lines_.back().append(v);
  }
}

bool PolygonMerge::do_merge(std::vector<glm::vec2> const& p1,
                            std::vector<glm::vec2> const& p2) {
  add_polygon(p1, &rect_1_);
  add_polygon(p2, &rect_2_);

  if (!rect_1_.intersects(rect_2_)) {
    if (rect_1_.contains(rect_2_)) {
      // all is rect 1
      result_.emplace_back(p1);
      return true;
    } else if (rect_2_.contains(rect_1_)) {
      // all is rect 2
      result_.emplace_back(p2);
      return true;
    } else {
      return false;
    }
  }

  build_mesh();

  merge_vertices();

  simplify_mesh();

  merge_mesh();
}

void PolygonMerge::build_mesh() {
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

void PolygonMerge::merge_vertices() {
  if (!mesh_.head) {
    // mesh is empty
    return;
  }

  for (auto v = mesh_.head->next; v;) {
    auto next = v->next;

    if (VertexCompare::Compare(v->point, v->prev->point)) {
      // already sorted, this means this two points is same
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

Edge* PolygonMerge::make_edge(Vertex* t, Vertex* b) {
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

bool PolygonMerge::check_intersection(Edge* left, Edge* right,
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

bool PolygonMerge::split_edge(Edge* edge, Vertex* v, ActiveEdgeList* ael,
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

bool PolygonMerge::intersect_pair_edge(Edge* left, Edge* right,
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

void PolygonMerge::remove_inner_edges() {
  for (auto v = mesh_.head; v; v = v->next) {
    for (auto e = v->edge_below.head; e;) {
      auto next = e->below_next;

      bool removed = false;

      while (next && next->top->point == e->top->point &&
             next->bottom->point == e->bottom->point) {
        removed = true;

        auto temp = next;
        next = next->below_next;
        temp->disconnect();
      }

      if (removed) {
        e->disconnect();
      }

      e = next;
    }
  }

  ActiveEdgeList ael{};

  for (auto v = mesh_.head; v; v = v->next) {
    if (!v->is_connected()) {
      continue;
    }

    Edge* left_enclosing = nullptr;
    Edge* right_enclosing = nullptr;

    ael.find_enclosing(v, &left_enclosing, &right_enclosing);

    bool prev_filled =
        left_enclosing && match_filltype(left_enclosing->winding);

    for (auto* e = v->edge_above.head; e;) {
      auto next = e->above_next;
      ael.remove(e);

      bool filled = match_filltype(e->winding);

      if (filled == prev_filled) {
        e->disconnect();
      }

      prev_filled = filled;
      e = next;
    }

    auto prev = left_enclosing;

    for (auto e = v->edge_below.head; e; e = e->below_next) {
      if (prev) {
        e->winding += prev->winding;
      }

      ael.insert(e, prev);
      prev = e;
    }
  }
}

void PolygonMerge::extract_boundary(Edge* e) {
  Path& path = result_.back();

  bool down = e->winding & 1;

  auto start = down ? e->top : e->bottom;
  path.emplace_back(start->point);

  do {
    e->winding = down ? 1 : -1;

    if (down) {
      path.emplace_back(e->bottom->point);
    } else {
      path.emplace_back(e->top->point);
    }

    Edge* next;

    if (down) {
      if ((next = e->above_next)) {
        down = false;
      } else if ((next = e->bottom->edge_below.tail)) {
        down = true;
      } else if ((next = e->above_prev)) {
        down = false;
      }
    } else {
      if ((next = e->below_prev)) {
        down = true;
      } else if ((next = e->top->edge_above.head)) {
        down = false;
      } else if ((next = e->below_next)) {
        down = true;
      }
    }

    e->disconnect();
    e = next;

  } while (e && (down ? e->top : e->bottom) != start);

  for (auto const& p : path) {
    std::cout << "\t p : " << glm::to_string(p) << std::endl;
  }
}

bool PolygonMerge::match_filltype(int32_t winding) {
  if (fill_type_ == FillType::kWinding) {
    return winding != 0;
  } else {
    return winding & 1;
  }
}

void PolygonMerge::begin_path() { result_.emplace_back(Path{}); }

void PolygonMerge::simplify_mesh() {
  ActiveEdgeList ael;

  for (Vertex* v = mesh_.head; v != nullptr; v = v->next) {
    if (!v->is_connected()) {
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

void PolygonMerge::merge_mesh() {
  remove_inner_edges();

  for (auto v = mesh_.head; v; v = v->next) {
    while (v->edge_below.head) {
      std::cout << "begin at : [" << glm::to_string(mesh_.head->point) << "]"
                << std::endl;
      begin_path();
      extract_boundary(v->edge_below.head);
    }
  }
}

}  // namespace TRM