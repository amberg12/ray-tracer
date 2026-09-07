import std;

import ppm;
import vec;
import scene;

using namespace raytracer;

auto main() -> int {
  std::vector<std::shared_ptr<scene::Object>> objects;
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(0.0, 0.0, -1.0), 0.5));
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(0.0, -100.5, -1.0), 100));

  constexpr auto aspect_ratio = 16.0 / 9.0;
  const auto writer = ppm::Ppm::create<scene::Scene>(
    static_cast<std::uint64_t>(255.0 * aspect_ratio), 255, objects);

  writer.render();
}
