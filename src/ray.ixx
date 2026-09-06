export module ray;

import std;
import vec;

export namespace raytracer::ray {
  struct Ray {
    Ray(const vec::Point3 &origin, const vec::Point3 &direction) : origin_(origin), direction_(direction) {
    }

    [[nodiscard]] auto origin() const -> const vec::Point3 & { return origin_; }

    [[nodiscard]] auto direction() const -> const vec::Vec3 & { return direction_; }

    auto at(const double t) const -> vec::Point3 {
      return origin_ + direction_ * t;
    }

  private:
    vec::Point3 origin_;
    vec::Vec3 direction_;
  };
}
