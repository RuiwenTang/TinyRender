/**
 *                   The FreeType Project LICENSE
 *                   ----------------------------

 *                           2006-Jan-27

 *                   Copyright 1996-2002, 2006 by
 *         David Turner, Robert Wilhelm, and Werner Lemberg



 * Introduction
 * ============

 * The FreeType  Project is distributed in  several archive packages;
 * some of them may contain, in addition to the FreeType font engine,
 * various tools and  contributions which rely on, or  relate to, the
 * FreeType Project.

 * This  license applies  to all  files found  in such  packages, and
 * which do not  fall under their own explicit  license.  The license
 * affects  thus  the  FreeType   font  engine,  the  test  programs,
 * documentation and makefiles, at the very least.

 * This  license   was  inspired  by  the  BSD,   Artistic,  and  IJG
 * (Independent JPEG  Group) licenses, which  all encourage inclusion
 * and  use of  free  software in  commercial  and freeware  products
 * alike.  As a consequence, its main points are that:

 *   o We don't promise that this software works. However, we will be
 *     interested in any kind of bug reports. (`as is' distribution)

 *   o You can  use this software for whatever you  want, in parts or
 *      full form, without having to pay us. (`royalty-free' usage)

 *    o You may not pretend that  you wrote this software.  If you use
 *      it, or  only parts of it,  in a program,  you must acknowledge
 *     somewhere  in  your  documentation  that  you  have  used  the
 *     FreeType code. (`credits')

 * We  specifically  permit  and  encourage  the  inclusion  of  this
 * software, with  or without modifications,  in commercial products.
 * We  disclaim  all warranties  covering  The  FreeType Project  and
 * assume no liability related to The FreeType Project.


 *  Finally,  many  people  asked  us  for  a  preferred  form  for  a
 *  credit/disclaimer to use in compliance with this license.  We thus
 * encourage you to use the following text:

 *   """
 *    Portions of this software are copyright � <year> The FreeType
 *    Project (www.freetype.org).  All rights reserved.
 *   """

 *  Please replace <year> with the value from the FreeType version you
 *  actually use.

* Legal Terms
* ===========

* 0. Definitions
* --------------

*   Throughout this license,  the terms `package', `FreeType Project',
*   and  `FreeType  archive' refer  to  the  set  of files  originally
*   distributed  by the  authors  (David Turner,  Robert Wilhelm,  and
*   Werner Lemberg) as the `FreeType Project', be they named as alpha,
*   beta or final release.

*   `You' refers to  the licensee, or person using  the project, where
*   `using' is a generic term including compiling the project's source
*   code as  well as linking it  to form a  `program' or `executable'.
*   This  program is  referred to  as  `a program  using the  FreeType
*   engine'.

*   This  license applies  to all  files distributed  in  the original
*   FreeType  Project,   including  all  source   code,  binaries  and
*   documentation,  unless  otherwise  stated   in  the  file  in  its
*   original, unmodified form as  distributed in the original archive.
*   If you are  unsure whether or not a particular  file is covered by
*   this license, you must contact us to verify this.

*   The FreeType  Project is copyright (C) 1996-2000  by David Turner,
*   Robert Wilhelm, and Werner Lemberg.  All rights reserved except as
*   specified below.

* 1. No Warranty
* --------------

*   THE FREETYPE PROJECT  IS PROVIDED `AS IS' WITHOUT  WARRANTY OF ANY
*   KIND, EITHER  EXPRESS OR IMPLIED,  INCLUDING, BUT NOT  LIMITED TO,
*   WARRANTIES  OF  MERCHANTABILITY   AND  FITNESS  FOR  A  PARTICULAR
*   PURPOSE.  IN NO EVENT WILL ANY OF THE AUTHORS OR COPYRIGHT HOLDERS
*   BE LIABLE  FOR ANY DAMAGES CAUSED  BY THE USE OR  THE INABILITY TO
*   USE, OF THE FREETYPE PROJECT.

* 2. Redistribution
* -----------------

*   This  license  grants  a  worldwide, royalty-free,  perpetual  and
*   irrevocable right  and license to use,  execute, perform, compile,
*   display,  copy,   create  derivative  works   of,  distribute  and
*   sublicense the  FreeType Project (in  both source and  object code
*   forms)  and  derivative works  thereof  for  any  purpose; and  to
*   authorize others  to exercise  some or all  of the  rights granted
*   herein, subject to the following conditions:

*    o Redistribution of  source code  must retain this  license file
*      (`FTL.TXT') unaltered; any  additions, deletions or changes to
*      the original  files must be clearly  indicated in accompanying
*      documentation.   The  copyright   notices  of  the  unaltered,
*      original  files must  be  preserved in  all  copies of  source
*      files.

*    o Redistribution in binary form must provide a  disclaimer  that
*      states  that  the software is based in part of the work of the
*      FreeType Team,  in  the  distribution  documentation.  We also
*      encourage you to put an URL to the FreeType web page  in  your
*      documentation, though this isn't mandatory.

*  These conditions  apply to any  software derived from or  based on
*  the FreeType Project,  not just the unmodified files.   If you use
*  our work, you  must acknowledge us.  However, no  fee need be paid
*  to us.

* 3. Advertising
* --------------

*  Neither the  FreeType authors and  contributors nor you  shall use
*  the name of the  other for commercial, advertising, or promotional
*  purposes without specific prior written permission.

*  We suggest,  but do not require, that  you use one or  more of the
*  following phrases to refer  to this software in your documentation
*  or advertising  materials: `FreeType Project',  `FreeType Engine',
*  `FreeType library', or `FreeType Distribution'.

*  As  you have  not signed  this license,  you are  not  required to
*  accept  it.   However,  as  the FreeType  Project  is  copyrighted
*  material, only  this license, or  another one contracted  with the
*  authors, grants you  the right to use, distribute,  and modify it.
*  Therefore,  by  using,  distributing,  or modifying  the  FreeType
*  Project, you indicate that you understand and accept all the terms
*  of this license.

* 4. Contacts
* -----------

*  There are two mailing lists related to FreeType:

*    o freetype@nongnu.org

*      Discusses general use and applications of FreeType, as well as
*      future and  wanted additions to the  library and distribution.
*      If  you are looking  for support,  start in  this list  if you
*      haven't found anything to help you in the documentation.

*    o freetype-devel@nongnu.org

*      Discusses bugs,  as well  as engine internals,  design issues,
*      specific licenses, porting, etc.

*  Our home page can be found at

*    http://www.freetype.org
*/

// the core calculation is based on Freetype ftgrays.c

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

template <class T>
uint64_t left_shift(T a, int32_t b) {
  return static_cast<uint64_t>(a) << b;
}

static inline int32_t trunc(int32_t x) { return x >> PIXEL_BITS; }
static inline int32_t sub_pixels(int32_t x) { return x << PIXEL_BITS; }
// static inline int32_t u_div_prep(int32_t t) {
//   return static_cast<int32_t>(std::numeric_limits<uint32_t>::max() >>
//                               PIXEL_BITS) /
//          t;
// }
// static inline int32_t u_div(int32_t a, int32_t b) {
//   return static_cast<int32_t>(
//       (static_cast<uint32_t>(a) * static_cast<uint32_t>(b)) >>
//       (sizeof(int32_t) * 8 - PIXEL_BITS));
// }

static void sw_split_conic(Vec2i* base) {
  int32_t a, b;

  base[4].x = base[2].x;
  a = base[0].x + base[1].x;
  b = base[1].x + base[2].x;
  base[3].x = b >> 1;
  base[2].x = (a + b) >> 2;
  base[1].x = a >> 1;

  base[4].y = base[2].y;
  a = base[0].y + base[1].y;
  b = base[1].y + base[2].y;
  base[3].y = b >> 1;
  base[2].y = (a + b) >> 2;
  base[1].y = a >> 1;
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

void SWRaster::quad_to(float cx, float cy, float x2, float y2) {
  render_quad_to(cx, cy, x2, y2);
}

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
    // int32_t dx_r = u_div_prep(dx);
    // int32_t dy_r = u_div_prep(dy);

    /* The fundamental value `prod' determines which side and the  */
    /* exact coordinate where the line exits current cell.  It is  */
    /* also easily updated when moving from one cell to the next.  */
    do {
      if (prod <= 0 && prod - dx * ONE_PIXEL > 0) {  // left
        fx2 = 0;
        // fy2 = u_div(-prod, -dx_r);
        fy2 = -1 * (0 * dx - prod) / dx;
        prod -= dy * ONE_PIXEL;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = ONE_PIXEL;
        fy1 = fy2;
        ex1--;
      } else if (prod - dx * ONE_PIXEL <= 0 &&
                 prod - dx * ONE_PIXEL + dy * ONE_PIXEL > 0) {  // up
        prod -= dx * ONE_PIXEL;
        // fx2 = u_div(-prod, dy_r);
        fx2 = -1 * prod / dy;
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
        // fy2 = u_div(prod, dx_r);
        fy2 = prod / dx;
        this->curr_cell.cover += (fy2 - fy1);
        this->curr_cell.area += (fy2 - fy1) * (fx1 + fx2);
        fx1 = 0;
        fy1 = fy2;
        ex1++;
      } else {  // down
        // fx2 = u_div(prod, -dy_r);
        fx2 = (0 * dx - prod) / dy;
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

void SWRaster::render_quad_to(float cx, float cy, float x2, float y2) {
  auto levels = m_lev_stack.data();
  auto arc = m_bez_stack.data();
  size_t level = 0;
  int32_t min, max, y;

  arc[0].x = up_scale(x2);
  arc[0].y = up_scale(y2);
  arc[1].x = up_scale(cx);
  arc[1].y = up_scale(cy);
  arc[2].x = m_x;
  arc[2].y = m_y;
  int32_t top = 0;

  int32_t dx = std::abs(arc[2].x + arc[0].x - 2 * arc[1].x);
  int32_t dy = std::abs(arc[2].y + arc[0].y - 2 * arc[1].y);

  if (dx < dy) {
    dx = dy;
  }

  if (dx < ONE_PIXEL / 4) {
    goto DRAW;
  }

  level = 0;
  do {
    dx >>= 2;
    level++;
  } while (dx > ONE_PIXEL / 4);

  levels[0] = level;

  do {
    level = levels[top];
    if (level > 0) {
      sw_split_conic(arc);
      arc += 2;
      top++;
      levels[top] = levels[top - 1] = level - 1;
      continue;
    }
  DRAW:
    render_line(arc[0].x, arc[0].y);
    top--;
    arc -= 2;
  } while (top >= 0);
}

void SWRaster::render_quad_to2(float cx, float cy, float x2, float y2) {
  Vec2i p0, p1, p2;
  int32_t ax, ay, bx, by, dx, dy;
  int32_t shift;

  int64_t rx, ry;
  int64_t qx, qy;
  int64_t px, py;

  uint32_t count;

  p0.x = m_x;
  p0.y = m_y;
  p1.x = up_scale(cx);
  p1.y = up_scale(cy);
  p2.x = up_scale(x2);
  p2.y = up_scale(y2);

  bx = p1.x - p0.x;
  by = p1.y - p0.y;
  ax = p2.x - p1.x - bx;
  ay = p2.y - p1.y - by;

  dx = std::abs(ax);
  dy = std::abs(ay);

  if (dx < dy) {
    dx = dy;
  }

  if (dx <= ONE_PIXEL / 4) {
    render_line(p2.x, p2.y);
    return;
  }

  shift = 0;

  do {
    dx >>= 2;
    shift += 1;
  } while (dx > ONE_PIXEL / 4);

  rx = left_shift(ax, 33 - 2 * shift);
  ry = left_shift(ay, 33 - 2 * shift);

  qx = left_shift(bx, 33 - shift) + left_shift(ax, 32 - 2 * shift);
  qy = left_shift(by, 33 - shift) + left_shift(ay, 32 - 2 * shift);

  px = left_shift(p0.x, 32);
  py = left_shift(p0.y, 32);

  for (count = 1U << shift; count > 0; count--) {
    px += qx;
    py += qy;
    qx += rx;
    qy += ry;

    render_line(px >> 32, py >> 32);
  }
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