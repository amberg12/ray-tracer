import std;

import ppm;
import vec;
import scene;

auto main() -> int {
  constexpr auto aspect_ratio = 16.0 / 9.0;
  const auto writer = raytracer::ppm::Ppm::create<raytracer::scene::Scene>(
    static_cast<std::uint64_t>(255.0 * aspect_ratio), 255);

  writer.render();
}
