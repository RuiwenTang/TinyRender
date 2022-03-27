#pragma once
#include <Color.h>

#include <vector>

namespace TRM {

class Bitmap {
 public:
  Bitmap(uint32_t width, uint32_t height);
  ~Bitmap() = default;

  void SetPixel(uint32_t x, uint32_t y, Color const& pixel);

  const Color* Pixels() const { return fPixels.data(); }

  void Clear() { ClearWithColor(Color::ColorTransparent()); }

  void ClearWithColor(Color const& color);

  void ReadFromFile(const char* filename);

  void WriteToPng(const char* filename);
  void WriteToTGA(const char* filename);

  uint32_t width() const { return fWidth; }

  uint32_t height() const { return fHeight; }

  void FlipHorizontally();

  void FlipVertically();

 private:
  uint32_t fWidth;
  uint32_t fHeight;
  std::vector<Color> fPixels = {};
};

}  // namespace TRM