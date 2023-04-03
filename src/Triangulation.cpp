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

}  // namespace TRM
