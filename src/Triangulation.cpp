#include <Triangulation.h>

#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>

namespace TRM {

Triangulation::Triangulation() : heap_(), out_lines_(), mesh_() {}

void Triangulation::set_filltype(FillType type) { fill_type_ = type; }

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

  simplify_mesh();

  tess_mesh();

  // output triangles
  for (auto poly = poly_list_; poly; poly = poly->next) {
    if (!match_filltype(poly)) {
      continue;
    }

    if (poly->count < 3) {
      continue;
    }

    for (auto m = poly->head; m; m = m->next) {
      emit_poly(m, callback);
    }
  }
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
    }
    ael->rewind(current, top ? top : v);

    split_edge(left, v, ael, current);
    split_edge(right, v, ael, current);
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

Polygon* Triangulation::make_poly(Vertex* v, int32_t winding) {
  auto poly = heap_.Allocate<Polygon>(v, winding);

  poly->next = poly_list_;
  poly_list_ = poly;

  return poly;
}

void Triangulation::simplify_mesh() {
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

void Triangulation::tess_mesh() {
  ActiveEdgeList ael;

  for (auto v = mesh_.head; v != nullptr; v = v->next) {
    if (!v->is_connected()) {
      continue;
    }

    std::cout << "=======================" << std::endl;

    std::cout << "current = " << glm::to_string(v->point) << std::endl;

    Edge* left_enclosing;
    Edge* right_enclosing;

    ael.find_enclosing(v, &left_enclosing, &right_enclosing);

    /**
     *
     *                   ...
     *                      \
     *      left_poly      head
     *                          v
     *
     */
    Polygon* left_poly = nullptr;
    /**
     *
     *              ...
     *         /
     *       tail     right_poly
     *     v
     *
     */
    Polygon* right_poly = nullptr;

    if (v->edge_above.head) {
      left_poly = v->edge_above.head->left_poly;
      right_poly = v->edge_above.tail->right_poly;
    } else {
      left_poly = left_enclosing ? left_enclosing->right_poly : nullptr;
      right_poly = right_enclosing ? right_enclosing->left_poly : nullptr;
    }

    if (v->edge_above.head) {
      // add above edge first
      if (left_poly) {
        left_poly =
            left_poly->add_edge(v->edge_above.head, Side::kRight, &heap_);
      }

      if (right_poly) {
        right_poly =
            right_poly->add_edge(v->edge_above.tail, Side::kLeft, &heap_);
      }

      for (auto e = v->edge_above.head; e != v->edge_above.tail;
           e = e->above_next) {
        auto right_edge = e->above_next;

        ael.remove(e);

        if (e->right_poly) {
          e->right_poly->add_edge(e, Side::kLeft, &heap_);
        }

        if (right_edge->left_poly && right_edge->left_poly != e->right_poly) {
          right_edge->left_poly->add_edge(e, Side::kRight, &heap_);
        }
      }

      ael.remove(v->edge_above.tail);
      if (!v->edge_below.head) {
        if (left_poly && right_poly && left_poly != right_poly) {
          left_poly->parent = right_poly;
          right_poly->parent = left_poly;
        }
      }
    }

    if (v->edge_below.head) {
      if (!v->edge_above.head) {
        if (left_poly && right_poly) {
          if (left_poly == right_poly) {
            if (left_poly->tail && left_poly->tail->side == Side::kLeft) {
              left_poly =
                  make_poly(left_poly->last_vertex(), left_poly->winding);

              left_enclosing->right_poly = left_poly;
            } else {
              right_poly =
                  make_poly(right_poly->last_vertex(), right_poly->winding);

              right_enclosing->left_poly = right_poly;
            }
          }

          auto join = heap_.Allocate<Edge>(left_poly->last_vertex(), v, 1);

          left_poly = left_poly->add_edge(join, Side::kRight, &heap_);
          right_poly = right_poly->add_edge(join, Side::kLeft, &heap_);
        }
      }

      auto left_edge = v->edge_below.head;
      left_edge->left_poly = left_poly;

      ael.insert(left_edge, left_enclosing);

      for (auto right_edge = left_edge->below_next; right_edge;
           right_edge = right_edge->below_next) {
        ael.insert(right_edge, left_edge);

        int32_t winding =
            left_edge->left_poly ? left_edge->left_poly->winding : 0;
        winding += left_edge->winding;

        if (winding != 0) {
          auto poly = make_poly(v, winding);

          left_edge->right_poly = right_edge->left_poly = poly;
        }

        left_edge = right_edge;
      }

      v->edge_below.tail->right_poly = right_poly;
    }
  }
}

bool Triangulation::match_filltype(Polygon* poly) {
  if (fill_type_ == FillType::kWinding) {
    return poly->winding != 0;
  } else {
    return (poly->winding & 0x01) != 0;
  }
}

void Triangulation::emit_poly(
    MonotonePolygon* poly,
    const std::function<void(const glm::vec2&, const glm::vec2&,
                             const glm::vec2&)>& callback) {
  auto e = poly->first;

  VertexList vs;

  vs.append(e->top);

  int32_t count = 1;

  while (e != nullptr) {
    if (poly->side == Side::kRight) {
      vs.append(e->bottom);
      e = e->right_poly_next;
    } else {
      vs.prepend(e->bottom);
      e = e->left_poly_next;
    }
    count++;
  }

  auto first = vs.head;
  auto v = first->next;
  while (v != vs.tail) {
    auto prev = v->prev;
    auto curr = v;
    auto next = v->next;

    if (count == 3) {
      callback(prev->point, v->point, next->point);
      return;
    }

    double ax = static_cast<double>(curr->point.x) - prev->point.x;
    double ay = static_cast<double>(curr->point.y) - prev->point.y;
    double bx = static_cast<double>(next->point.x) - curr->point.x;
    double by = static_cast<double>(next->point.y) - curr->point.y;

    if (ax * by - ay * bx >= 0.0) {
      callback(prev->point, curr->point, next->point);
      v->prev->next = v->next;
      v->next->prev = v->prev;

      count--;

      if (v->prev == first) {
        v = v->next;
      } else {
        v = v->prev;
      }
    } else {
      v = v->next;
    }
  }
}

}  // namespace TRM
