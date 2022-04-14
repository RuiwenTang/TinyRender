#pragma once

#include <Geometry.h>

#include <array>
#include <limits>
#include <unordered_map>
#include <vector>

namespace TRM {

struct Span {
  int32_t x;
  int32_t y;
  int32_t len;
  int32_t cover;
};

class SWRaster {
  struct Cell {
    int32_t x;
    int32_t cover;
    int32_t area;
  };

 public:
  SWRaster() = default;

  ~SWRaster() = default;

  int32_t min_y() const { return m_min_ey; }
  int32_t max_y() const { return m_max_ey; }

  void move_to(float x, float y);
  void line_to(float x2, float y2);
  void quad_to(float cx, float cy, float x2, float y2);
  void cubic_to(float cx1, float cy1, float cx2, float cy2, float x3, float y3);

  // generate spans
  void sweep();

  const std::vector<Span>& current_spans() const { return m_spans; }

 private:
  // start a new cell at given position
  void start_cell(int32_t ex, int32_t ey);
  // move current cell to next position
  void set_cell(int32_t ex, int32_t ey);
  // render a straight line across multiple cells in any direction
  void render_line(int32_t to_x, int32_t to_y);
  // save current cell to m_cells
  void record_curr_cell();
  Cell* find_cell();

  void update_xy(int32_t ex, int32_t ey);

  void sweep_hline(int32_t x, int32_t y, int32_t area, int32_t count);

 private:
  Cell curr_cell = {};
  std::unordered_map<int32_t, std::vector<Cell>> m_cells_y = {};
  std::vector<Span> m_spans = {};
  std::array<Vec2i, 32 * 3 + 1> m_bez_stack = {};
  std::array<size_t, 32> m_lev_stack = {};
  bool curr_cell_invalid = true;
  bool even_odd = false;
  int32_t m_x = -1;
  int32_t m_y = -1;
  int32_t m_ex = -1;
  int32_t m_ey = -1;
  int32_t m_min_ex = std::numeric_limits<int32_t>::max();
  int32_t m_min_ey = std::numeric_limits<int32_t>::max();
  int32_t m_max_ex = std::numeric_limits<int32_t>::min();
  int32_t m_max_ey = std::numeric_limits<int32_t>::min();
};

}  // namespace TRM