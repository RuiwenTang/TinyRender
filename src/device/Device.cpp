#include <Device.h>

#include "device/BitmapDevice.h"

namespace TRM {

std::unique_ptr<Device> Device::CreateBitmapDevice(
    std::shared_ptr<Bitmap> bitmap) {
  return std::make_unique<BitmapDevice>(bitmap);
}

}  // namespace TRM