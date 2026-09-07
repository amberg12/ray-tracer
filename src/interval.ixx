export module interval;

import std;

export namespace raytracer::interval {
  struct Interval {
    double min = std::numeric_limits<double>::min();
    double max = std::numeric_limits<double>::max();

    [[nodiscard]] constexpr auto size() const -> double {
      return max - min;
    }

    [[nodiscard]] constexpr auto contains(const double x) const -> bool {
      return min <= x && x <= max;
    }

    [[nodiscard]] constexpr auto surrounds(const double x) const -> bool {
      return min < x && x < max;
    }

    inline static const Interval empty, universe;
  };

  const auto Interval::empty{
    std::numeric_limits<double>::max(), std::numeric_limits<double>::min()
  };

  const auto Interval::universe{
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max()
  };
}
