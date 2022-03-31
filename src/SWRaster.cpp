#include <SWRaster.h>

#include <algorithm>

namespace TRM {

enum {
  PIXEL_BITS = 8,
  ONE_PIXEL = 1 << PIXEL_BITS,
  PIXEL_MAXK = ONE_PIXEL - 1,
};

template <class T>
int32_t up_scale(T value) {
  static int32_t scale = 1 << (PIXEL_BITS);
  return static_cast<int32_t>(value * scale);
}

template <class T>
int32_t down_scale(T value) {
  return static_cast<int32_t>(value) >> (PIXEL_BITS);
}

static inline int32_t trunc(int32_t x) { return x >> PIXEL_BITS; }
static inline int32_t sub_pixels(int32_t x) { return x << PIXEL_BITS; }
static inline int32_t u_div_prep(int32_t t) {
  return static_cast<int32_t>(std::numeric_limits<uint32_t>::max() >>
                              PIXEL_BITS) /
         t;
}
static inline int32_t u_div(int32_t a, int32_t b) {
  return static_cast<int32_t>(
      (static_cast<uint32_t>(a) * static_cast<uint32_t>(b)) >>
      (sizeof(int32_t) * 8 - PIXEL_BITS));
}

void SWRaster::move_to(float x, float y) {
  int32_t pos_x, pos_y;

  if (!curr_cell_invalid) record_curr_cell();

  pos_x = up_scale(x);
  pos_y = up_scale(y);

  start_cell(trunc(pos_x), trunc(pos_y));

  this->m_x = pos_x;
  this->m_y = pos_y;
}

void SWRaster::line_to(float x2, float y2) {
  render_line(up_scale(x2), up_scale(y2));
}

void SWRaster::quad_to(float cx, float cy, float x2, float y2) {}

void SWRaster::cubic_to(float cx1, float cy1, float cx2, float cy2, float x3,
                        float y3) {}

void SWRaster::sweep() {
  int32_t current_y;

  m_spans.clear();

  if (m_cells_y.empty()) {
    return;
  }

  for (current_y = m_min_ex; current_y <= m_max_ey; current_y++) {
    if (m_cells_y.count(current_y) == 0) {
      // this can not happen
      continue;
    }
    auto const& list = m_cells_y[current_y];

    int32_t cover = 0;
    int32_t x = m_min_ex;

    for (auto const& cell : list) {
      int32_t area;
      if (cell.x > x && cover != 0) {
        sweep_hline(x, current_y, cover * (ONE_PIXEL * 2), cell.x - x);
      }

      cover += cell.cover;
      area = cover * (ONE_PIXEL * 2) - cell.area;

      if (area != 0 && cell.x >= m_min_ex) {
        sweep_hline(cell.x, current_y, area, 1);
      }

      x = cell.x + 1;
    }
  }
}

void SWRaster::render_line(int32_t to_x, int32_t to_y) {
  int32_t dx, dy, fx1, fy1, fx2, fy2;
  int32_t ex1, ex2, ey1, ey2;

  ex1 = trunc(this->m_x);
  ex2 = trunc(to_x);
  ey1 = trunc(this->m_y);
  ey2 = trunc(to_y);

  dx = to_x - this->m_x;
  dy = to_y - this->m_y;

  fx1 = this->m_x - sub_pixels(ex1);
  fy1 = this->m_y - sub_pixels(ey1);

  if (ex1 == ex2 && ey1 == ey2) {  // inside one cell

  } else if (dy == 0) {  // any horizontal line
    ex1 = ex2;
    set_cell(ex1, ey1);
  } else if (dx == 0) {
    if (dy > 0) {  // vertical line up
      do {
        fy2 = ONE_PIXEL;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * fx1 * 2;
        fy1 = 0;
        ey1++;
        set_cell(ex1, ey1);
      } while (ey1 != ey2);
    } else {  // vertical line down
      do {
        fy2 = 0;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * fx1 * 2;
        fy1 = ONE_PIXEL;
        ey1--;
        set_cell(ex1, ey1);
      } while (ey1 != ey2);
    }
  } else {  // any other line
    int32_t prod = dx * fy1 - dy * fx1;
    int32_t dx_r = u_div_prep(dx);
    int32_t dy_r = u_div_prep(dy);

    /* The fundamental value `prod' determines which side and the  */
    /* exact coordinate where the line exits current cell.  It is  */
    /* also easily updated when moving from one cell to the next.  */
    do {
      if (prod <= 0 && prod - dx * ONE_PIXEL > 0) {  // left
        fx2 = 0;
        fy2 = u_div(-prod, -dx_r);
        prod -= dy * ONE_PIXEL;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = ONE_PIXEL;
        fy1 = fy2;
        ex1--;
      } else if (prod - dx * ONE_PIXEL <= 0 &&
                 prod - dx * ONE_PIXEL + dy * ONE_PIXEL > 0) {  // up
        prod -= dx * ONE_PIXEL;
        fx2 = u_div(-prod, dy_r);
        fy2 = ONE_PIXEL;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = fx2;
        fy1 = 0;
        ey1++;
      } else if (prod - dx * ONE_PIXEL + dy * ONE_PIXEL <= 0 &&
                 prod + dy * ONE_PIXEL >= 0) {  // right

        prod += dy * ONE_PIXEL;
        fx2 = ONE_PIXEL;
        fy2 = u_div(prod, dx_r);
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = 0;
        fy1 = fy2;
        ex1++;
      } else {  // down
        fx2 = u_div(prod, -dy_r);
        fy2 = 0;
        prod += dx * ONE_PIXEL;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = fx2;
        fy1 = ONE_PIXEL;
        ey1--;
      }

      set_cell(ex1, ey1);
    } while (ex1 != ex2 || ey1 != ey2);
  }

  fx2 = to_x - sub_pixels(ex2);
  fy2 = to_y - sub_pixels(ey2);

  this->curr_cell.cover += (fy2 - fy1);
  this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);

  this->m_x = to_x;
  this->m_y = to_y;
}

void SWRaster::start_cell(int32_t ex, int32_t ey) {
  this->curr_cell.area = 0;
  this->curr_cell.cover = 0;
  this->update_xy(ex, ey);

  this->curr_cell_invalid = false;

  set_cell(ex, ey);
}

void SWRaster::set_cell(int32_t ex, int32_t ey) {
  // moving to a different cell
  if (ex != this->m_ex || ey != this->m_ey) {
    if (!this->curr_cell_invalid) {
      record_curr_cell();
    }

    this->curr_cell.area = 0;
    this->curr_cell.cover = 0;
    this->update_xy(ex, ey);
  }

  this->curr_cell_invalid = false;
}

void SWRaster::record_curr_cell() {
  if (this->curr_cell.area | this->curr_cell.cover) {
    auto cell = this->find_cell();
    cell->area += curr_cell.area;
    cell->cover += curr_cell.cover;
  }
}

SWRaster::Cell* SWRaster::find_cell() {
  if (m_cells_y.count(m_ey) == 0) {
    std::vector<Cell> list(1);
    m_cells_y[m_ey] = list;

    auto cell = &m_cells_y[m_ey][0];
    cell->x = m_ex;
    cell->area = 0;
    cell->cover = 0;
    return cell;
  }

  auto& list = m_cells_y[m_ey];

  int32_t index = -1;
  for (size_t i = 0; i < list.size(); i++) {
    if (list[i].x == m_ex) {
      return &list[i];
    }

    if (list[i].x > m_ex) {
      index = i;
      break;
    }
  }

  Cell* cell = nullptr;
  if (index != -1) {
    list.insert(list.begin() + index, Cell{});
    cell = &list[index];
  } else {
    list.emplace_back(Cell{});
    cell = &list.back();
  }

  cell->x = m_ex;
  cell->area = 0;
  cell->cover = 0;

  return cell;
}

void SWRaster::update_xy(int32_t ex, int32_t ey) {
  m_ex = ex;
  m_ey = ey;

  m_min_ex = std::min(m_min_ex, m_ex);
  m_min_ey = std::min(m_min_ey, m_ey);
  m_max_ex = std::max(m_max_ex, m_ex);
  m_max_ey = std::max(m_max_ey, m_ey);
}

void SWRaster::sweep_hline(int32_t x, int32_t y, int32_t area, int32_t count) {
  /* compute the coverage line's coverage, depending on the    */
  /* outline fill rule                                         */
  /*                                                           */
  /* the coverage percentage is area/(PIXEL_BITS*PIXEL_BITS*2) */
  /*                                                           */
  int32_t coverage = static_cast<int32_t>(area >> (PIXEL_BITS * 2 + 1 - 8));
  // use range [0, 256]
  if (coverage < 0) {
    coverage = -coverage;
  }

  if (this->even_odd) {
    coverage &= 511;
    if (coverage > 256)
      coverage = 512 - coverage;
    else if (coverage == 256)
      coverage = 255;
  } else {
    if (coverage >= 256) coverage = 255;
  }

  x = std::min(x, std::numeric_limits<int32_t>::max());
  y = std::min(y, std::numeric_limits<int32_t>::max());

  if (coverage) {
    if (!m_spans.empty()) {
      auto p_span = &m_spans.back();
      if (p_span->y == y && p_span->x + p_span->len == x &&
          p_span->cover == coverage) {
        p_span->len = p_span->len + count;
        return;
      }
    }

    Span span;
    span.x = x;
    span.y = y;
    span.len = count;
    span.cover = coverage;

    m_spans.emplace_back(span);
  }
}

}  // namespace TRM