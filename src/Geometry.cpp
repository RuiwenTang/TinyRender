#include <Geometry.h>

namespace TRM {

Matrix lookAt(Vec3f eye, Vec3f center, Vec3f up) {
  return glm::lookAt(eye, center, up);
}

Matrix perspective(float fovy, float aspect, float near, float far) {
  return glm::perspective(fovy, aspect, near, far);
}

float degToRadian(float deg) { return deg * PI / 180.f; }

void Vertex::insert_above(Edge* e) {
  if (e->top->point == e->bottom->point ||                     // no edge
      VertexCompare::Compare(e->bottom->point, e->top->point)  // not above
  ) {
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
  if (e->top->point == e->bottom->point ||                     // no edge
      VertexCompare::Compare(e->bottom->point, e->top->point)  // not below
  ) {
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

  double dx = static_cast<double>(other->top->point.x) - top->point.x;
  double dy = static_cast<double>(other->top->point.y) - top->point.y;

  double s_number = dy * other->le_b + dx * other->le_a;
  double t_number = dy * le_b + dx * le_a;

  if (denom > 0.0 ? (s_number < 0.0 || s_number > denom || t_number < 0.0 ||
                     t_number > denom)
                  : (s_number > 0.0 || s_number < denom || t_number > 0.0 ||
                     t_number < denom)) {
    return false;
  }

  double scale = 1.0 / denom;

  point->x = glm::round(
      static_cast<float>(top->point.x - s_number * le_b * scale));
  point->y = glm::round(
      static_cast<float>(top->point.y + s_number * le_a * scale));

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

static void remove_edge_above(Edge* edge) {
  LinkedList<Edge>::Remove<&Edge::above_prev, &Edge::above_next>(
      edge, &edge->bottom->edge_above.head, &edge->bottom->edge_above.tail);
}

static void remove_edge_below(Edge* edge) {
  LinkedList<Edge>::Remove<&Edge::below_prev, &Edge::below_next>(
      edge, &edge->top->edge_below.head, &edge->top->edge_below.tail);
}

void Edge::disconnect() {
  remove_edge_above(this);
  remove_edge_below(this);
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
  bottom->insert_above(this);
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

void MonotonePolygon::add_edge(Edge* edge) {
  if (side == Side::kRight) {
    LinkedList<Edge>::Insert<&Edge::right_poly_prev, &Edge::right_poly_next>(
        edge, last, nullptr, &first, &last);
  } else {
    LinkedList<Edge>::Insert<&Edge::left_poly_prev, &Edge::left_poly_next>(
        edge, last, nullptr, &first, &last);
  }
}

Polygon* Polygon::add_edge(Edge* e, Side side, ObjectHeap* heap) {
  auto p_parent = this->parent;
  auto poly = this;

  if (side == Side::kRight) {
    if (e->used_in_right) {
      return this;
    }
  } else {
    if (e->used_in_left) {
      return this;
    }
  }

  if (p_parent) {
    this->parent = p_parent->parent = nullptr;
  }

  if (!this->tail) {
    this->head = this->tail = heap->Allocate<MonotonePolygon>(e, side, winding);
    this->count += 2;
  } else if (e->bottom == this->tail->last->bottom) {
    // close this polygon
    return poly;
  } else if (side == this->tail->side) {
    this->tail->add_edge(e);
    this->count++;
  } else {
    e = heap->Allocate<Edge>(this->tail->last->bottom, e->bottom, 1);
    this->tail->add_edge(e);
    this->count++;

    if (p_parent) {
      p_parent->add_edge(e, side, heap);
      poly = p_parent;
    } else {
      auto m = heap->Allocate<MonotonePolygon>(e, side, this->winding);
      m->prev = this->tail;
      this->tail->next = m;
      this->tail = m;
    }
  }

  return poly;
}

Vertex* Polygon::last_vertex() const {
  if (tail) {
    return tail->last->bottom;
  }

  return first_vert;
}

}  // namespace TRM
