#include <RasterizerCells.h>

#include <algorithm>
#include <cmath>

namespace TRM {

void Cell::initial() {
  x = 0x7FFFFFFF;
  y = 0x7FFFFFFF;
  cover = 0;
  area = 0;
}

int Cell::not_equal(int32_t ex, int32_t ey) const {
  return (ex - x) | (ey - y);
}

bool Cell::operator<(const Cell& other) const {
  if (y < other.y) {
    return true;
  }

  if (y == other.y) {
    return x < other.x;
  }

  return false;
}

bool Cell::operator>(const Cell& other) const {
  if (y > other.y) {
    return true;
  }

  if (y == other.y) {
    return x > other.x;
  }

  return false;
}

bool Cell::operator==(const Cell& other) const {
  return x == other.x && y == other.y;
}

void RasterizerCells::reset() {
  m_num_cells = 0;
  m_curr_block = 0;
  m_curr_cell.initial();
  m_cells.clear();
  m_min_x = std::numeric_limits<int32_t>::max();
  m_min_y = std::numeric_limits<int32_t>::max();
  m_max_x = std::numeric_limits<int32_t>::min();
  m_max_y = std::numeric_limits<int32_t>::min();
  m_sorted = false;
}

void RasterizerCells::sort() {
  if (m_sorted) {
    return;
  }

  std::sort(m_cells.begin(), m_cells.end());

  m_sorted = true;
}

void RasterizerCells::line(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
  int32_t dx_limit = 16384 << poly_subpixel_shift;

  int32_t dx = x2 - x1;
  if (dx >= dx_limit || dx <= -dx_limit) {
    int32_t cx = (x1 + x2) >> 1;
    int32_t cy = (y1 + y2) >> 1;
    line(x1, y1, cx, cy);
    line(cx, cy, x2, y2);
  }

  int32_t dy = y2 - y1;
  int32_t ex1 = x1 >> poly_subpixel_shift;
  int32_t ex2 = x2 >> poly_subpixel_shift;
  int32_t ey1 = y1 >> poly_subpixel_shift;
  int32_t ey2 = y2 >> poly_subpixel_shift;
  int32_t fy1 = y1 & poly_subpixel_mask;
  int32_t fy2 = y2 & poly_subpixel_mask;

  m_min_x = std::min(std::min(ex1, ex2), m_min_x);
  m_max_x = std::max(std::max(ex1, ex2), m_max_x);
  m_min_y = std::min(std::min(ey1, ey2), m_min_y);
  m_max_y = std::max(std::max(ey1, ey2), m_max_y);

  set_curr_cell(ex1, ey1);

  // everything on a single hline
  if (ey1 == ey2) {
    render_hline(ey1, x1, fy1, x2, fy2);
    return;
  }

  // vertical line
  int32_t incr = 1;   // increment direction
  int32_t first = 0;  // first subpixel value
  int32_t x_from;     // start x posotion (poly scale shifted)
  int32_t delta;      // subpixel delta
  if (dx == 0) {
    int32_t ex = x1 >> poly_subpixel_shift;
    int32_t two_fx = (x1 - (ex << poly_subpixel_shift)) << 1;
    int32_t area;

    first = poly_subpixel_scale;
    if (dy < 0) {
      first = 0;
      incr = -1;
    }

    x_from = x1;
    delta = first - fy1;
    m_curr_cell.cover += delta;
    m_curr_cell.area += two_fx * delta;

    ey1 += incr;
    set_curr_cell(ex, ey1);

    delta = first + first - poly_subpixel_scale;
    area = two_fx * delta;
    while (ey1 != ey2) {
      m_curr_cell.cover = delta;
      m_curr_cell.area = area;
      ey1 += incr;
      set_curr_cell(ex, ey1);
    }

    delta = fy2 - poly_subpixel_scale + first;
    m_curr_cell.cover += delta;
    m_curr_cell.area += two_fx * delta;
    return;
  }

  int32_t p = (poly_subpixel_scale - fy1) * dx;  // subpixel x-axis
  first = poly_subpixel_scale;

  if (dy < 0) {
    p = fy1 * dx;
    first = 0;
    incr = -1;
    dy = -dy;
  }

  delta = p / dy;
  int32_t mod = p % dy;  // subpixel mod
  if (mod < 0) {
    delta--;
    mod += dy;
  }

  x_from = x1 + delta;
  render_hline(ey1, x1, fy1, x_from, first);

  ey1 += incr;
  set_curr_cell(x_from >> poly_subpixel_shift, ey1);

  if (ey1 != ey2) {
    p = poly_subpixel_scale * dx;
    int32_t lift = p / dy;
    int32_t rem = p % dy;

    if (rem < 0) {
      lift--;
      rem += dy;
    }
    mod -= dy;

    while (ey1 != ey2) {
      delta = lift;
      mod += rem;
      if (mod >= 0) {
        mod -= dy;
        delta++;
      }

      int32_t x_to = x_from + delta;
      render_hline(ey1, x_from, poly_subpixel_scale - first, x_to, first);

      x_from = x_to;

      ey1 += incr;
      set_curr_cell(x_from >> poly_subpixel_shift, ey1);
    }
  }
  render_hline(ey1, x_from, poly_subpixel_scale - first, x2, fy2);
}

void RasterizerCells::set_curr_cell(int32_t x, int32_t y) {
  if (m_curr_cell.not_equal(x, y)) {
    add_curr_cell();
    m_curr_cell.x = x;
    m_curr_cell.y = y;
    m_curr_cell.cover = 0;
    m_curr_cell.area = 0;
  }
}

void RasterizerCells::add_curr_cell() { m_cells.emplace_back(m_curr_cell); }

void RasterizerCells::render_hline(int32_t ey, int32_t x1, int32_t y1,
                                   int32_t x2, int32_t y2) {
  int32_t ex1 = x1 >> poly_subpixel_shift;
  int32_t ex2 = x2 >> poly_subpixel_shift;
  int32_t fx1 = x1 & poly_subpixel_mask;
  int32_t fx2 = x2 & poly_subpixel_mask;

  if (y1 == y2) {
    set_curr_cell(ex2, ey);
    return;
  }

  // same x-axis but with different y-axis subpixel value
  if (ex1 == ex2) {
    int32_t delta = y2 - y1;
    m_curr_cell.cover += delta;
    m_curr_cell.area += (fx1 + fx2) * delta;
    return;
  }

  int32_t p = (poly_subpixel_scale - fx1) * (y2 - y1);
  int32_t first = poly_subpixel_scale;
  int32_t incr = 1;

  int32_t dx = x2 - x1;

  if (dx < 0) {
    p = fx1 * (y2 - y1);
    first = 0;
    incr = -1;
    dx = -dx;
  }

  int32_t delta = p / dx;
  int32_t mod = p % dx;

  if (mod < 0) {
    delta--;
    mod += dx;
  }

  m_curr_cell.cover += delta;
  m_curr_cell.area += (fx1 + first) * delta;

  ex1 += incr;
  set_curr_cell(ex1, ey);
  y1 += delta;

  if (ex1 != ex2) {
    p = poly_subpixel_scale * (y2 - y1 + delta);
    int32_t lift = p / dx;
    int32_t rem = p % dx;

    if (rem < 0) {
      lift--;
      rem += dx;
    }

    mod -= dx;

    while (ex1 != ex2) {
      delta = lift;
      mod += rem;

      if (mod >= 0) {
        mod -= dx;
        delta++;
      }

      m_curr_cell.cover += delta;
      m_curr_cell.area += poly_subpixel_scale * delta;
      y1 += delta;
      ex1 += incr;
      set_curr_cell(ex1, ey);
    }
  }

  delta = y2 - y1;
  m_curr_cell.cover += delta;
  m_curr_cell.area += (fx2 + poly_subpixel_scale - first) * delta;
}

uint32_t RasterizerCells::calculate_alpha(int32_t area) {
  int32_t cover = area >> (poly_subpixel_shift * 2 + 1 - aa_shift);
  if (cover < 0) {
    cover = -cover;
  }

  if (cover > aa_mask) {
    cover = aa_mask;
  }

  return cover;
}

}  // namespace TRM