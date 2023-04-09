#include <Bitmap.h>
#include <v_ft_raster.h>

#include <iostream>
#include <memory>
#include <vector>

#define WIDTH 800
#define HEIGHT 600

struct FTOutline {
  SW_FT_Outline ft;

  FTOutline()
      : ft(),
        m_points(100),
        m_tags(100),
        m_contours(100),
        m_counter_flags(100) {
    ft.points = m_points.data();
    ft.tags = (char*)m_tags.data();
    ft.contours = m_contours.data();
    ft.contours_flag = (char*)m_counter_flags.data();
  }

  SW_FT_Pos to_ft_coord(float x) { return SW_FT_Pos(x * 64.f); }

  void move_to(float x, float y) {
    SW_FT_Vector p;

    ft.points[ft.n_points].x = to_ft_coord(x);
    ft.points[ft.n_points].y = to_ft_coord(y);
    ft.tags[ft.n_points] = SW_FT_CURVE_TAG_ON;
    if (ft.n_points) {
      ft.contours[ft.n_contours] = ft.n_points - 1;
      ft.n_contours++;
    }
    // mark the current contour as open
    // will be updated if ther is a close tag at the end.
    ft.contours_flag[ft.n_contours] = 1;

    ft.n_points++;
  }

  void line_to(float x, float y) {
    ft.points[ft.n_points].x = to_ft_coord(x);
    ft.points[ft.n_points].y = to_ft_coord(y);
    ft.tags[ft.n_points] = SW_FT_CURVE_TAG_ON;
    ft.n_points++;
  }

  void quad_to(float cx, float cy, float x2, float y2) {
    ft.points[ft.n_points].x = to_ft_coord(cx);
    ft.points[ft.n_points].y = to_ft_coord(cy);
    ft.tags[ft.n_points] = SW_FT_CURVE_TAG_CONIC;
    ft.n_points++;

    ft.points[ft.n_points].x = to_ft_coord(x2);
    ft.points[ft.n_points].y = to_ft_coord(y2);
    ft.tags[ft.n_points] = SW_FT_CURVE_TAG_ON;
    ft.n_points++;
  }

  void close() {
    // mark the contour as a close path.
    ft.contours_flag[ft.n_contours] = 0;

    int index;
    if (ft.n_contours) {
      index = ft.contours[ft.n_contours - 1] + 1;
    } else {
      index = 0;
    }

    // make sure atleast 1 point exists in the segment.
    if (ft.n_points == index) {
      m_closed = false;
      return;
    }

    ft.points[ft.n_points].x = ft.points[index].x;
    ft.points[ft.n_points].y = ft.points[index].y;
    ft.tags[ft.n_points] = SW_FT_CURVE_TAG_ON;
    ft.n_points++;
  }

  void end() {
    if (ft.n_points) {
      ft.contours[ft.n_contours] = ft.n_points - 1;
      ft.n_contours++;
    }
  }

 private:
  bool m_closed = false;
  std::vector<SW_FT_Vector> m_points;
  std::vector<uint8_t> m_tags;
  std::vector<int16_t> m_contours;
  std::vector<uint8_t> m_counter_flags;
};

struct SpanHolder {
  std::vector<SW_FT_Span> spans = {};
};

void ft_raster_generation_cb(int count, const SW_FT_Span* spans, void* user) {
  SpanHolder* holder = (SpanHolder*)user;

  for (int i = 0; i < count; i++) {
    holder->spans.emplace_back(spans[i]);
  }
}

void ft_bbox_cb(int x, int y, int w, int h, void* user) {}

int main(int argc, const char** argv) {
  std::shared_ptr<TRM::Bitmap> framebuffer =
      std::make_shared<TRM::Bitmap>(WIDTH, HEIGHT);

  framebuffer->ClearWithColor(TRM::Color(255, 255, 255, 255));

  FTOutline outline;

  outline.move_to(10, 10);
  outline.quad_to(160, 10, 160, 160);
  outline.line_to(10, 10);

  outline.end();

  SpanHolder holder;

  SW_FT_Raster_Params params;
  params.flags = SW_FT_RASTER_FLAG_DIRECT | SW_FT_RASTER_FLAG_AA;
  params.gray_spans = &ft_raster_generation_cb;
  params.bbox_cb = &ft_bbox_cb;
  params.user = &holder;
  params.source = &outline.ft;

  sw_ft_grays_raster.raster_render(nullptr, &params);

  for (auto const& span : holder.spans) {
    int32_t x = span.x;
    int32_t y = span.y;
    for (int32_t i = 0; i < span.len; i++) {
      framebuffer->BlendPixel(x + i, y, TRM::Color(0, 0, 0, span.coverage));
    }
  }

  framebuffer->WriteToPng("ft_raster_test.png");
  return 0;
}
