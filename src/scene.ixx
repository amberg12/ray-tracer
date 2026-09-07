export module scene;

import std;
import ppm;
import vec;
import ray;

export namespace raytracer::scene {
  struct HitRecord {
    vec::Point3 point;
    vec::Vec3 normal;
    double t;
    bool front_face;

    auto set_face_normal(const ray::Ray &r, const vec::Vec3 &outward_normal) -> void {
      front_face = r.direction().dot(outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
  };

  struct Object {
    virtual ~Object() = default;

    [[nodiscard]] virtual auto hit(const ray::Ray &r, double ray_t_min, double ray_t_max, HitRecord &rec) -> bool = 0;
  };

  struct Sphere : public Object {
    Sphere(const vec::Point3 &center, const double radius) : center_(center), radius_(radius) {
    }

    [[nodiscard]] auto hit(const ray::Ray &r, const double ray_t_min, const double ray_t_max,
                           HitRecord &rec) -> bool override {
      const vec::Vec3 o = center_ - r.origin();
      const auto a = std::pow(r.direction().length(), 2);
      const auto h = r.direction().dot(o);
      const auto c = std::pow(o.length(), 2) - std::pow(radius_, 2);

      const auto discriminant = h * h - a * c;

      if (discriminant < 0) {
        return false;
      }

      const auto sqrt_d = std::sqrt(discriminant);

      auto root = (h - sqrt_d) / a;

      const auto invalid_root = [&] {
        return root <= ray_t_min || ray_t_max <= root;
      };

      if (invalid_root()) {
        root = (h + sqrt_d) / a;

        if (invalid_root()) {
          return false;
        }
      }

      rec.t = root;
      rec.point = r.at(rec.t);
      const vec::Vec3 outward_normal = (rec.point - center_) / radius_;
      rec.set_face_normal(r, outward_normal);

      return true;
    }

  private:
    vec::Point3 center_;
    double radius_;
  };

  struct Scene : public ppm::PpmWriter {
    Scene(const std::uint64_t image_width, const std::uint64_t image_height,
          const std::vector<std::shared_ptr<Object> > &objects)
      : image_width_(image_width), image_height_(image_height), objects_(objects) {
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
      const auto pixel_centre = pixel00_loc_ + pixel_delta_u_ * static_cast<double>(x) + pixel_delta_v_ * static_cast<
                                  double>(y);
      const auto ray_direction = pixel_centre - camera_centre_;

      const auto r = ray::Ray{camera_centre_, ray_direction};

      const auto ray_color = [&] {
        HitRecord rec{};

        if (hit(r, 0, std::numeric_limits<double>::max(), rec)) {
          return 0.5 * (rec.normal + vec::Color{1.0, 1.0, 1.0});
        }

        const auto unit_vector = r.direction().unit_vector();
        const auto a = 0.5 * unit_vector.y() + 1.0;
        return (1.0 - a) * vec::Color(1.0, 1.0, 1.0) + a * vec::Color(0.5, 0.7, 1.0);
      }();

      return ray_color.normalize();
    }

  private:
    auto hit(const ray::Ray &r, const double ray_t_min, const double ray_t_max, HitRecord &rec) const -> bool {
      HitRecord temp_record{};
      bool hit_anything = false;
      auto closest_so_far = ray_t_max;

      for (const auto &object: objects_) {
        if (object->hit(r, ray_t_min, closest_so_far, temp_record)) {
          hit_anything = true;
          closest_so_far = temp_record.t;
          rec = temp_record;
        }
      }

      return hit_anything;
    }

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

    std::vector<std::shared_ptr<Object> > objects_;
  };
}
