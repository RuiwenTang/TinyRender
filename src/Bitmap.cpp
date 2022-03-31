#include <Bitmap.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef _MSC_VER
#define STBI_MSC_SECURE_CRT
#endif
#include <stb_image_write.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <algorithm>
#include <cassert>

namespace TRM {

Bitmap::Bitmap(uint32_t width, uint32_t height)
    : fWidth(width), fHeight(height) {
  fPixels.resize(width * height);
}

Color Bitmap::GetPixel(uint32_t x, uint32_t y) const {
  if (x < 0 || x >= fWidth || y < 0 || y >= fHeight) {
    return {};
  }

  return fPixels[x + y * fWidth];
}

void Bitmap::SetPixel(uint32_t x, uint32_t y, Color const &pixel) {
  if (x < 0 || x >= fWidth || y < 0 || y >= fHeight) {
    return;
  }
  fPixels[x + y * fWidth] = pixel;
}

void Bitmap::BlendPixel(uint32_t x, uint32_t y, Color const &pixel) {
  if (x < 0 || x >= fWidth || y < 0 || y >= fHeight) {
    return;
  }

  auto src = this->GetPixel(x, y);

  auto result = src.blend(pixel);

  SetPixel(x, y, result);
}

void Bitmap::ClearWithColor(Color const &color) {
  std::for_each(fPixels.begin(), fPixels.end(),
                [color](Color &c) { c = color; });
}

void Bitmap::ReadFromFile(const char *filename) {
  int x, y, n;
  unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
  fPixels.resize(x * y);
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      Color color;
      std::memcpy(color.rgba().data(), data + (j * x + i) * n, n);
      if (n < 4) {
        color.rgba()[3] = 255;
      }
      fPixels[j * x + i] = color;
    }
  }
}

void Bitmap::WriteToPng(const char *filename) {
  stbi_write_png(filename, fWidth, fHeight, 4, fPixels.data(), 0);
}

void Bitmap::WriteToTGA(const char *filename) {
  stbi_write_jpg(filename, fWidth, fHeight, 4, fPixels.data(), 50);
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