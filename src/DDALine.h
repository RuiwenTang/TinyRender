#include <cmath>
#include <cstdint>

namespace TRM {

class dda2_line_interpolator {
 public:
  typedef int save_data_type;
  enum save_size_e { save_size = 2 };

  dda2_line_interpolator() = default;

  dda2_line_interpolator(int y1, int y2, int count)
      : m_cnt(count <= 0 ? 1 : count),
        m_lft((y2 - y1) / m_cnt),
        m_rem((y2 - y1) % m_cnt),
        m_mod(m_rem),
        m_y(y1) {
    if (m_mod <= 0) {
      m_mod += count;
      m_rem += count;
      m_lft--;
    }
    m_mod -= count;
  }

  dda2_line_interpolator(int y1, int y2, int count, int)
      : m_cnt(count <= 0 ? 1 : count),
        m_lft((y2 - y1) / m_cnt),
        m_rem((y2 - y1) % m_cnt),
        m_mod(m_rem),
        m_y(y1) {
    if (m_mod <= 0) {
      m_mod += count;
      m_rem += count;
      m_lft--;
    }
  }

  void save(save_data_type* data) const {
    data[0] = m_mod;
    data[1] = m_y;
  }

  void load(const save_data_type* data) {
    m_mod = data[0];
    m_y = data[1];
  }

  void operator++() {
    m_mod += m_rem;
    m_y += m_lft;
    if (m_mod > 0) {
      m_mod -= m_cnt;
      m_y++;
    }
  }

  void operator--() {
    if (m_mod <= m_rem) {
      m_mod += m_cnt;
      m_y--;
    }
    m_mod -= m_rem;
    m_y -= m_lft;
  }

  void adjust_forward() { m_mod -= m_cnt; }

  int mod() const { return m_mod; }
  int rem() const { return m_rem; }
  int lft() const { return m_lft; }

  int y() const { return m_y; }

 private:
  int m_cnt = {0};
  int m_lft = {0};
  int m_rem = {0};
  int m_mod = {0};
  int m_y = {0};
};

class line_bresenham_interpolator {
 public:
  enum subpixel_scale_e {
    subpixel_shift = 8,
    subpixel_scale = 1 << subpixel_shift,
    subpixel_mask = subpixel_scale - 1
  };

  static int line_lr(int v) { return v >> subpixel_shift; }

  explicit line_bresenham_interpolator(int x1, int y1, int x2, int y2)
      : m_x1_lr(line_lr(x1)),
        m_y1_lr(line_lr(y1)),
        m_x2_lr(line_lr(x2)),
        m_y2_lr(line_lr(y2)),
        m_ver(std::abs(m_x2_lr - m_x1_lr) < std::abs(m_y2_lr - m_y1_lr)),
        m_len(m_ver ? std::abs(m_y2_lr - m_y1_lr)
                    : std::abs(m_x2_lr - m_x1_lr)),
        m_inc(m_ver ? ((y2 > y1) ? 1 : -1) : ((x2 > x1) ? 1 : -1)),
        m_interpolator(m_ver ? x1 : y1, m_ver ? x2 : y2, m_len) {}

  bool is_ver() const { return m_ver; }
  unsigned len() const { return m_len; }
  int inc() const { return m_inc; }

  void h_step() {
    ++m_interpolator;
    m_x1_lr += m_inc;
  }

  void v_step() {
    ++m_interpolator;
    m_y1_lr += m_inc;
  }

  int x1() const { return m_x1_lr; }
  int y1() const { return m_y1_lr; }
  int x2() const { return line_lr(m_interpolator.y()); }
  int y2() const { return line_lr(m_interpolator.y()); }
  int x2_hr() const { return m_interpolator.y(); }
  int y2_hr() const { return m_interpolator.y(); }

 private:
  int m_x1_lr;
  int m_y1_lr;
  int m_x2_lr;
  int m_y2_lr;
  bool m_ver;
  unsigned m_len;
  int m_inc;
  dda2_line_interpolator m_interpolator;
};

}  // namespace TRM