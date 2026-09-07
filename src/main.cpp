import std;

import ppm;
import vec;

namespace {
  struct ExampleWriter : public raytracer::ppm::PpmWriter {
    ~ExampleWriter() override = default;

    ExampleWriter(const std::uint64_t image_width, const std::uint64_t image_height)
      : image_width{image_width}, image_height{image_height} {
    }


    auto write(const std::uint64_t x, const std::uint64_t y) -> raytracer::vec::PpmColor override {
      const auto nx = static_cast<double>(x) / static_cast<double>(image_width);
      const auto ny = static_cast<double>(y) / static_cast<double>(image_height);

      return raytracer::vec::Vec2{nx, ny}.expand<3>().normalize();
    }

    std::uint64_t image_width, image_height;
  };
}

auto main() -> int {
  const auto writer = raytracer::ppm::Ppm::create<ExampleWriter>(255, 255);

  writer.render();
}
