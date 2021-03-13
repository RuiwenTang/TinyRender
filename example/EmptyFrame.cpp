#include <Bitmap.h>

static TRM::Color WHITE(255, 255, 255);
static TRM::Color RED(255, 0, 0);

int main(int argc, const char** argv) {
  TRM::Bitmap image(800, 600);

  image.ClearWithColor(RED);

  image.WriteToPng("framebuffer.png");
  return 0;
}