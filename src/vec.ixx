export module vec;

import std;

export namespace raytracer::vec {
  template<class T, std::size_t N>
  struct Vec {
    Vec() requires(std::is_default_constructible_v<T>) = default;

    template<class... Args>
      requires((std::convertible_to<Args, T> && ...) && sizeof...(Args) == N)
    explicit Vec(Args... args) : raw{std::forward<Args>(args)...} {
    }

    template<std::size_t NewSize>
      requires(NewSize > N && std::is_default_constructible_v<T>)
    auto expand() const -> Vec<T, NewSize> {
      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return Vec<T, NewSize>{
          (I < N ? raw[I] : T{})...
        };
      }(std::make_index_sequence<NewSize>{});
    }

    template<class F, class... Vs>
    auto apply(const F &f, const Vec<Vs, N> &... vs) const -> Vec<std::invoke_result_t<F &, T, Vs...>, N> {
      const auto apply_at = [&]<std::size_t I>() {
        return std::invoke(f, raw[I], vs.raw[I]...);
      };

      return [&]<std::size_t... I>(std::index_sequence<I...>) {
        return Vec<std::invoke_result_t<F &, T, Vs...>, N>{
          apply_at.template operator()<I>()...
        };
      }(std::make_index_sequence<N>{});
    }

    static constexpr auto size() -> std::size_t {
      return N;
    }

    template<std::size_t Idx>
      requires(Idx < N)
    auto get() const -> const T & {
      return raw[Idx];
    }

    template<std::size_t Idx>
      requires(Idx < N)
    auto get() -> T & {
      return raw[Idx];
    }

    auto normalize() const -> Vec<std::uint8_t, N> requires (std::same_as<T, double>) {
      const auto normalize = [](const double n) -> std::uint8_t {
        return static_cast<std::uint8_t>(n * 255.999);
      };

      return apply(normalize);
    }

    friend auto operator+(const Vec &rhs, const Vec &lhs)
      requires requires(T a, T b) { a + b; } {
      return rhs.apply(std::plus{}, lhs);
    }

    friend auto operator*(const Vec &lhs, const double rhs)
      requires requires(T a) { a * rhs; } {
      return lhs.apply([rhs](T v) {
        return v * rhs;
      });
    }

    std::array<T, N> raw;
  };

  using Vec2 = Vec<double, 2>;
  using Vec3 = Vec<double, 3>;
  using Point3 = Vec<double, 3>;
  using Color = Vec<double, 3>;
  using PpmColor = Vec<std::uint8_t, 3>;
}

export template<class T, std::size_t N>
struct std::formatter<raytracer::vec::Vec<T, N> > : formatter<std::string_view> {
  auto format(
    const raytracer::vec::Vec<T, N> &v,
    std::format_context &ctx
  ) const {
    auto out = ctx.out();

    for (std::size_t i = 0; i < N; ++i) {
      if (i != 0)
        *out++ = ' ';

      out = std::format_to(out, "{}", v.raw[i]);
    }

    return out;
  }
};
