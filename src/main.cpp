import std;

import ppm;
import vec;
import scene;

auto main() -> int {
  const auto writer = raytracer::ppm::Ppm::create<raytracer::scene::Scene>(255, 255);

  writer.render();
}
