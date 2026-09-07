export module interval;

import std;

export namespace raytracer::interval {
  struct Interval {
    constexpr Interval(const double min, const double max) : min{min}, max{max} {
    }

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

    [[nodiscard]] constexpr auto clamp(const double x) const -> double {
      // We must account for the case of max < min
      return std::max(min, std::min(x, max));
    }

    static const Interval empty, universe;
  };

  const Interval Interval::empty{
    std::numeric_limits<double>::max(), std::numeric_limits<double>::min()
  };

  const Interval Interval::universe{
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max()
  };
}
