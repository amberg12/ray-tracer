export module scene;

import std;
import ppm;
import vec;
import ray;

export namespace raytracer::scene {
  struct Scene : public ppm::PpmWriter {
    Scene(const std::uint64_t image_width, const std::uint64_t image_height)
      : image_width_(image_width), image_height_(image_height) {
      viewport_height_ = 2.0;
      viewport_width_ = viewport_height_ * static_cast<double>(image_width_) / static_cast<double>(image_height_);

      camera_centre_ = vec::Point3{0.0, 0.0, 0.0};

      viewport_u_ = vec::Vec3{viewport_width_, 0.0, 0.0};
      viewport_v_ = vec::Vec3{0.0, -viewport_height_, 0.0};

      pixel_delta_u_ = viewport_u_ / static_cast<double>(image_width_);
      pixel_delta_v_ = viewport_v_ / static_cast<double>(image_height_);

      viewport_upper_left_ = camera_centre_ - vec::Vec3{0.0, 0.0, focal_length} - viewport_u_ / 2 - viewport_v_ / 2;
      pixel00_loc_ = viewport_upper_left_ + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
    }

    auto write(const std::uint64_t x, const std::uint64_t y) -> vec::PpmColor override {
      const auto pixel_centre = pixel00_loc_ + pixel_delta_u_ * x + pixel_delta_v_ * y;
      const auto ray_direction = pixel_centre - camera_centre_;

      const auto r = ray::Ray{pixel_centre, ray_direction};

      const auto ray_color = [&] {
        const auto unit_vector = r.direction().unit_vector();
        const auto a = 0.5 * unit_vector.y() + 1.0;
        return (1.0 - a) * vec::Color(1.0, 1.0, 1.0) + a * vec::Color(0.5, 0.7, 1.0);
      }();

      return ray_color.normalize();
    }

  private:
    static constexpr double focal_length = 1.0;

    const std::uint64_t image_width_{};
    const std::uint64_t image_height_{};

    double viewport_height_{};
    double viewport_width_{};

    vec::Point3 camera_centre_;

    vec::Vec3 viewport_u_;
    vec::Vec3 viewport_v_;

    vec::Vec3 pixel_delta_u_{};
    vec::Vec3 pixel_delta_v_{};

    vec::Vec3 viewport_upper_left_{};
    vec::Vec3 pixel00_loc_{};
  };
}
