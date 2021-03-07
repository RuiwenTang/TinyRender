#include <Bitmap.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef _MSC_VER
#define STBI_MSC_SECURE_CRT
#endif
#include <stb_image_write.h>

#include <algorithm>
#include <cassert>

namespace TRM {

Bitmap::Bitmap(uint32_t width, uint32_t height)
    : fWidth(width), fHeight(height) {
  fPixels.resize(width * height);
}

void Bitmap::ClearWithColor(Color const &color) {
  std::for_each(fPixels.begin(), fPixels.end(),
                [color](Color &c) { c = color; });
}

void Bitmap::WriteToPng(const char *filename) {
  stbi_write_png(filename, fWidth, fHeight, 4, fPixels.data(), 0);
}

void Bitmap::FlipHorizontally() {
  if (fWidth * fHeight <= 0) {
    return;
  }

  int32_t half_width = fWidth >> 1;
  for (int32_t i = 0; i < half_width; i++) {
    for (int32_t j = 0; j < fHeight; j++) {
      std::swap(fPixels[i + j * fWidth], fPixels[fWidth - 1 - i + j * fWidth]);
    }
  }
}

void Bitmap::FlipVertically() {
  if (fWidth * fHeight <= 0) {
    return;
  }

  uint32_t bytes_per_line = fWidth * sizeof(Color);

  std::vector<Color> pixel_line(fWidth);

  int32_t half_height = fHeight >> 1;

  uint8_t *data = (uint8_t *)fPixels.data();
  for (int j = 0; j < half_height; j++) {
    uint32_t l1 = j * bytes_per_line;
    uint32_t l2 = (fHeight - 1 - j) * bytes_per_line;

    std::memmove((void *)pixel_line.data(), (void *)(data + l1),
                 bytes_per_line);
    std::memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
    std::memmove((void *)(data + l2), (void *)pixel_line.data(),
                 bytes_per_line);
  }
}

}  // namespace TRM