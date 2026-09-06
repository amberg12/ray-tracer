import std;

import ppm;

struct ExampleWriter : public raytracer::PpmWriter {
  ~ExampleWriter() override = default;

  ExampleWriter(const std::uint64_t image_width, const std::uint64_t image_height)
    : image_width{image_width}, image_height{image_height} {
  }


  auto write(const std::uint64_t x,
             const std::uint64_t y) -> std::tuple<std::uint8_t, std::uint8_t, std::uint8_t> override {
    const auto r = static_cast<std::uint8_t>(x * 255 / image_width);
    const auto g = static_cast<std::uint8_t>(y * 255 / image_height);
    return {r, g, 0};
  }

  std::uint64_t image_width, image_height;
};

auto main() -> int {
  const auto writer = raytracer::Ppm::create<ExampleWriter>(255, 255);

  writer.render();
}
