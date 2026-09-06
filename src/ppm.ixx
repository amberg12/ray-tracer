export module ppm;

import std;
import vec;

export namespace raytracer::ppm {
  struct PpmWriter {
    virtual ~PpmWriter() = default;

    virtual auto write(std::uint64_t x, std::uint64_t y) -> vec::PpmColor = 0;
  };

  struct Ppm {
    template<class Writer, class... Args>
      requires(std::derived_from<Writer, PpmWriter> && std::constructible_from<Writer, std::uint64_t, std::uint64_t,
                 Args...>)
    static auto create(const std::uint64_t image_width, const std::uint64_t image_height, Args... args) -> Ppm {
      auto writer = std::make_unique<Writer>(image_width, image_height, std::forward<Args>(args)...);

      return {image_width, image_height, std::move(writer)};
    }

    auto render() const -> void {
      std::println("P3");
      std::println("{} {}", image_width_, image_height_);
      std::println("255");

      for (const auto y: std::views::iota(static_cast<std::uint64_t>(0), image_height_)) {
        std::print(std::clog, "\r\033[2KLines left: {}", image_height_ - y);
        std::flush(std::clog);

        for (const auto x: std::views::iota(static_cast<std::uint64_t>(0), image_width_)) {
          const auto color = writer_->write(x, y);
          std::println("{}", color);
        }
      }

      std::println(std::clog);
    }

  private:
    Ppm(const std::uint64_t image_width, const std::uint64_t image_height, std::unique_ptr<PpmWriter> writer)
      : image_width_(image_width), image_height_(image_height), writer_(std::move(writer)) {
    }

    const std::uint64_t image_width_;
    const std::uint64_t image_height_;
    const std::unique_ptr<PpmWriter> writer_;
  };
}
