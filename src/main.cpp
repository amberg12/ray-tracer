import std;

import ppm;
import vec;
import scene;

using namespace raytracer;

auto main() -> int {
  auto red_metal = std::make_shared<scene::Metal>(vec::Color{0.8, 0.0, 0.0});
  auto green_metal = std::make_shared<scene::Metal>(vec::Color{0.0, 0.8, 0.0});
  auto blue_lambertian = std::make_shared<scene::Lambertian>(vec::Color{0.2, 0.2, 0.6});
  auto yellow_lambertian = std::make_shared<scene::Lambertian>(vec::Color{0.8, 0.8, 0.1});

  std::vector<std::shared_ptr<scene::Object>> objects;
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(0.0, 0.0, -1.0), 0.5, blue_lambertian));
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(1.2, 0.0, -1.0), 0.5, red_metal));
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(-1.2, 0.0, -1.0), 0.5, green_metal));
  objects.push_back(std::make_unique<scene::Sphere>(vec::Vec3(0.0, -100.5, -1.0), 100, yellow_lambertian));

  constexpr auto aspect_ratio = 16.0 / 9.0;
  const auto writer = ppm::Ppm::create<scene::Scene>(
    static_cast<std::uint64_t>(255.0 * aspect_ratio), 255, objects);

  writer.render();
}
