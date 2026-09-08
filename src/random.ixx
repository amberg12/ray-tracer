export module random;

import std;

export namespace raytracer::random {
  [[nodiscard]] auto random_double() -> double {
    static std::uniform_real_distribution distribution{0.0, 1.0};
    static std::mt19937 generator;
    return distribution(generator);
  }

  [[nodiscard]] double random_double(const double min, const double max) {
    return min + (max - min) * random_double();
  }
}
