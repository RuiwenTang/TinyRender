#pragma once

#include <limits>
#include <vector>

namespace TRM {

enum {
  poly_subpixel_shift = 8,
  poly_subpixel_scale = 1 << poly_subpixel_shift,
  poly_subpixel_mask = poly_subpixel_scale - 1,
  aa_shift = 8,
  aa_scale = 1 << aa_shift,
  aa_mask = aa_scale - 1,
  aa_scale2 = aa_scale * 2,
  aa_mask2 = aa_scale2 - 1,
};

struct Cell {
  int32_t x;
  int32_t y;
  int32_t cover;
  int32_t area;

  Cell() { initial(); }
  Cell(const Cell&) = default;
  Cell& operator=(const Cell&) = default;

  void initial();

  int not_equal(int32_t ex, int32_t ey) const;

  bool operator<(const Cell& other) const;
  bool operator>(const Cell& other) const;
  bool operator==(const Cell& other) const;
};

class RasterizerCells {
  enum BlockScaleE {
    cell_blck_shift = 12,
    cell_block_size = 1 << cell_blck_shift,
    cell_block_mask = cell_block_size - 1,
    cell_block_pool = 256,
    cell_block_limit = 1024,
  };

 public:
  RasterizerCells() = default;
  ~RasterizerCells() = default;

  void reset();

  void sort();

  void line(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

  uint32_t calculate_alpha(int32_t area);

  const std::vector<Cell>& getCells() const { return m_cells; }

 private:
  void set_curr_cell(int32_t x, int32_t y);
  void add_curr_cell();
  /**
   * @param ey y-axis position of element
   * @param x1 x-axis start position (poly scale shifted)
   * @param y1 y-axis start position (subpixel value)
   * @param x2 x-axis end position (poly scale shifted)
   * @param y2 y-axis end position (subpixel value)
   */
  void render_hline(int32_t ey, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

 private:
  uint32_t m_num_blocks = 0;
  uint32_t m_max_blocks = 0;
  uint32_t m_curr_block = 0;
  uint32_t m_num_cells = 0;
  Cell m_curr_cell = {};
  std::vector<Cell> m_cells = {};
  int32_t m_min_x = std::numeric_limits<int32_t>::max();
  int32_t m_max_x = std::numeric_limits<int32_t>::min();
  int32_t m_min_y = std::numeric_limits<int32_t>::max();
  int32_t m_max_y = std::numeric_limits<int32_t>::min();
  bool m_sorted = false;
};

}  // namespace TRM