export module scene;

import std;
import ppm;
import random;
import vec;
import ray;
import interval;

namespace {
  using namespace raytracer;

  [[nodiscard]] auto sample_square() -> vec::Vec3 {
    return vec::Vec3{random::random_double(-0.5, 0.5), random::random_double(-0.5, 0.5), 0.0};
  }
}

export namespace raytracer::scene {
  struct Material;

  struct HitRecord {
    vec::Point3 point;
    vec::Vec3 normal;
    double t;
    bool front_face;
    std::shared_ptr<Material> material;

    auto set_face_normal(const ray::Ray &r, const vec::Vec3 &outward_normal) -> void {
      front_face = r.direction().dot(outward_normal) < 0;
      normal = front_face ? outward_normal : -outward_normal;
    }
  };

  struct Material {
    struct Result {
      vec::Color attenuation;
      ray::Ray scattered;
    };

    virtual ~Material() = default;

    virtual auto scatter(const ray::Ray &in, const HitRecord &rec) -> std::optional<Result> {
      return std::nullopt;
    }
  };

  struct Lambertian : public Material {
    explicit Lambertian(const vec::Color &albedo) : Material(), albedo_(albedo) {
    }

    auto scatter(const ray::Ray &in, const HitRecord &rec) -> std::optional<Result> override {
      auto scatter_direction = rec.normal + vec::Vec3::random_unit();

      if (scatter_direction.near_zero()) {
        scatter_direction = rec.normal;
      }

      return Result{.attenuation = albedo_, .scattered = ray::Ray{rec.point, scatter_direction}};
    }

  private:
    vec::Color albedo_;
  };

  struct Metal : public Material {
    explicit Metal(const vec::Color &albedo) : Material(), albedo_(albedo) {
    }

    auto scatter(const ray::Ray &in, const HitRecord &rec) -> std::optional<Result> override {
      return Result{.attenuation = albedo_, .scattered = ray::Ray{rec.point, in.direction().reflect(rec.normal)}};
    }

  private:
    vec::Color albedo_;
  };

  struct Object {
    virtual ~Object() = default;

    [[nodiscard]] virtual auto hit(const ray::Ray &r, interval::Interval ray_t, HitRecord &rec) -> bool = 0;
  };

  struct Sphere : public Object {
    Sphere(const vec::Point3 &center, const double radius, std::shared_ptr<Material> material) : center_(center),
      radius_(radius), material_(material) {
    }

    [[nodiscard]] auto hit(const ray::Ray &r, const interval::Interval ray_t, HitRecord &rec) -> bool override {
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

      if (!ray_t.surrounds(root)) {
        root = (h + sqrt_d) / a;

        if (!ray_t.surrounds(root)) {
          return false;
        }
      }

      rec.t = root;
      rec.point = r.at(rec.t);
      const vec::Vec3 outward_normal = (rec.point - center_) / radius_;
      rec.set_face_normal(r, outward_normal);
      rec.material = material_;

      return true;
    }

  private:
    vec::Point3 center_;
    double radius_;
    std::shared_ptr<Material> material_;
  };

  struct Scene : public ppm::PpmWriter {
    Scene(const std::uint64_t image_width, const std::uint64_t image_height,
          const std::vector<std::shared_ptr<Object> > &objects)
      : image_width_(image_width), image_height_(image_height), objects_(objects) {
      viewport_height_ = 2.0;
      viewport_width_ = viewport_height_ * static_cast<double>(image_width_) / static_cast<double>(image_height_);

      camera_center_ = vec::Point3{0.0, 0.0, 0.0};

      viewport_u_ = vec::Vec3{viewport_width_, 0.0, 0.0};
      viewport_v_ = vec::Vec3{0.0, -viewport_height_, 0.0};

      pixel_delta_u_ = viewport_u_ / static_cast<double>(image_width_);
      pixel_delta_v_ = viewport_v_ / static_cast<double>(image_height_);

      viewport_upper_left_ = camera_center_ - vec::Vec3{0.0, 0.0, focal_length} - viewport_u_ / 2 - viewport_v_ / 2;
      pixel00_loc_ = viewport_upper_left_ + 0.5 * (pixel_delta_u_ + pixel_delta_v_);
    }

    auto write(const std::uint64_t x, const std::uint64_t y) -> vec::PpmColor override {
      const auto pixel_color = std::ranges::fold_left(
        std::views::iota(0, sampling_rate)
        | std::views::transform([&](auto) { return ray_color(generate_ray(x, y), depth_limit); }),
        vec::Color{},
        std::plus{}
      );

      return (pixel_color * (1.0 / sampling_rate)).apply([](const auto v) { return std::sqrt(v); }).normalize();
    }

  private:
    auto ray_color(const ray::Ray &r, int depth) const -> vec::Color {
      // If a ray has bounced enough, we assume that it can no longer pick up light.
      if (depth <= 0) {
        return {};
      }

      if (HitRecord rec{}; hit(r, interval::Interval{0.001, std::numeric_limits<double>::max()}, rec)) {
        if (const auto result = rec.material->scatter(r, rec)) {
          return result->attenuation * ray_color(result->scattered, depth - 1);
        }
      }

      const auto unit_vector = r.direction().unit_vector();
      const auto a = 0.5 * unit_vector.y() + 1.0;
      return (1.0 - a) * vec::Color(1.0, 1.0, 1.0) + a * vec::Color(0.5, 0.7, 1.0);
    };

    auto hit(const ray::Ray &r, const interval::Interval ray_t, HitRecord &rec) const -> bool {
      HitRecord temp_record{};
      bool hit_anything = false;
      auto closest_so_far = ray_t.max;

      for (const auto &object: objects_) {
        if (object->hit(r, interval::Interval{ray_t.min, closest_so_far}, temp_record)) {
          hit_anything = true;
          closest_so_far = temp_record.t;
          rec = temp_record;
        }
      }

      return hit_anything;
    }

    [[nodiscard]] auto generate_ray(const std::uint64_t x, const std::uint64_t y) const -> ray::Ray {
      const auto offset = sample_square();
      const auto pixel_sample = pixel00_loc_ + ((x + offset.x()) * pixel_delta_u_) + (y + offset.y()) * pixel_delta_v_;

      const auto ray_origin = camera_center_;
      const auto ray_direction = pixel_sample - ray_origin;

      return {ray_origin, ray_direction};
    }

    static constexpr double focal_length = 1.0;
    static constexpr int sampling_rate = 10;
    static constexpr int depth_limit = 25;

    const std::uint64_t image_width_{};
    const std::uint64_t image_height_{};

    double viewport_height_{};
    double viewport_width_{};

    vec::Point3 camera_center_;

    vec::Vec3 viewport_u_;
    vec::Vec3 viewport_v_;

    vec::Vec3 pixel_delta_u_{};
    vec::Vec3 pixel_delta_v_{};

    vec::Vec3 viewport_upper_left_{};
    vec::Vec3 pixel00_loc_{};

    std::vector<std::shared_ptr<Object> > objects_;
  };
}
