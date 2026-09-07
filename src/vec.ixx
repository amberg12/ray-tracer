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

    template<class F>
    auto collect(const F &f, T base = T{}) const -> T {
      [&]<std::size_t... I>(std::index_sequence<I...>) {
        ((base = f(base, raw[I])), ...);
      }(std::make_index_sequence<N>{});

      return base;
    }

    auto sum() const -> T requires requires(T a, T b) { a + b; } {
      return collect(std::plus{});
    }

    static constexpr auto size() -> std::size_t {
      return N;
    }

    template<std::size_t Idx>
      requires(Idx < N)
    auto get() const -> const T & {
      return raw[Idx];
    }

    auto x() const -> const T & requires(N >= 1) {
      return get<0>();
    }

    auto y() const -> const T & requires(N >= 2) {
      return get<1>();
    }

    auto z() const -> const T & requires(N >= 3) {
      return get<2>();
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

    auto length() const -> T {
      return std::sqrt(apply([](T v) { return v * v; }).sum());
    }

    auto unit_vector() const -> Vec {
      return *this / length();
    }

    friend auto operator+(const Vec &lhs, const Vec &rhs)
      requires requires(T a, T b) { a + b; } {
      return lhs.apply(std::plus{}, rhs);
    }

    friend auto operator-(const Vec &lhs, const Vec &rhs)
      requires requires(T a, T b) { a - b; } {
      return lhs.apply(std::minus{}, rhs);
    }

    friend auto operator*(const Vec &lhs, const double rhs)
      requires requires(T a) { a * rhs; } {
      return lhs.apply([rhs](T v) { return v * rhs; });
    }

    friend auto operator*(const double lhs, const Vec &rhs)
      requires requires(T a) { lhs * a; } {
      return rhs.apply([lhs](T v) { return lhs * v; });
    }

    friend auto operator/(const Vec &lhs, const double rhs)
      requires requires(T a) { a / rhs; } {
      return lhs.apply([rhs](T v) { return v / rhs; });
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
