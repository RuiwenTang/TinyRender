#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <cassert>
#include <type_traits>
#include <tuple>
#include <array>

namespace TRM {

template<size_t DIM, typename T>
class vec {
 public:
  vec() {
    for (size_t i = 0; i < DIM; i++)
      data[i] = static_cast<T>(0);
  }
  template<class ... ARGS, typename = typename std::enable_if<std::tuple_size<std::tuple<ARGS...>>::value == DIM>::type>
  explicit vec(ARGS... args) {
    std::tuple<ARGS...> arg_tuple{args...};
    init_data(std::move(arg_tuple));
  }

  vec(const vec<DIM, T> &v1) {
    for (size_t i = 0; i < DIM; i++)
      data[i] = v1[i];
  }

  virtual ~vec() = default;

  template<size_t COL>
  explicit vec(const vec<COL, T> &v1) {
    size_t max = std::min(DIM, COL);
    for (size_t i = 0; i < max; i++)
      data[i] = v1[i];
  }

  T &operator[](size_t index) {
    assert(index < DIM);
    return data[index];
  }

  const T &operator[](size_t index) const {
    assert(index < DIM);
    return data[index];
  }

  vec<DIM, T> &operator=(const vec<DIM, T> &v1) {
    for (size_t i = 0; i < DIM; i++)
      data[i] = v1[i];
    return *this;
  }

  T norm() {
    T res(0);
    for (size_t i = 0; i < DIM; i++) {
      res += data[i] * data[i];
    }
    return std::sqrt(res);
  }

  vec<DIM, T> &normalize() {
    *this = (*this) * (static_cast<T>(1) / norm());
    return *this;
  }

 protected:
  std::array<T, DIM> data;

 private:
  template<class ... ARGS>
  void init_data(std::tuple<ARGS...> args) {
    set_data(args);
  }

  template<size_t I = 0, typename ... ARGS>
  typename std::enable_if<I == sizeof...(ARGS), void>::type set_data(std::tuple<ARGS...> &args) {

  }

  template<size_t I = 0, typename ... ARGS>
  typename std::enable_if<I < sizeof...(ARGS), void>::type set_data(std::tuple<ARGS...> &args) {
    data[I] = std::get<I>(args);
    set_data<I + 1, ARGS...>(args);
  }
};


template<size_t ROW, size_t COL, typename T>
class mat {
 public:
  mat() = default;
  mat(const mat<ROW, COL, T> &m1) {
    for (size_t i = 0; i < ROW; i++) {
      for (size_t j = 0; j < COL; j++) {
        data[i][j] = m1[i][j];
      }
    }
  }

  ~mat() = default;

  void identity() {
    for (size_t i = 0; i < ROW; i++)
      for (size_t j = 0; j < COL; j++)
        if (i == j) data[i][j] = static_cast<T>(1);
  }

  vec<COL, T> &operator[](const size_t index) {
    return data[index];
  }

  const vec<COL, T> &operator[](const size_t index) const {
    return data[index];
  }

  mat<ROW, COL, T> &operator=(const mat<ROW, COL, T> &m1) {
    for (size_t i = 0; i < ROW; i++) {
      for (size_t j = 0; j < COL; j++) {
        data[i][j] = m1[i][j];
      }
    }
    return *this;
  }

 protected:
  vec<COL, T> data[ROW];
};

template<size_t U, typename V>
V operator*(const vec<U, V> &v1, const vec<U, V> &v2) {
  V res(0);

  for (size_t i = 0; i < U; i++)
    res += v1[i] * v2[i];

  return res;
}

template<size_t U, typename V>
vec<U, V> operator*(const vec<U, V> &v1, V n) {
  vec<U, V> res;
  for (size_t i = 0; i < U; i++)
    res[i] = v1[i] * n;
  return res;
}

template<size_t U, typename V>
vec<U, V> operator*(V n, const vec<U, V> &v1) {
  vec<U, V> res;
  for (size_t i = 0; i < U; i++)
    res[i] = v1[i] * n;
  return res;
}

template<size_t ROW, size_t COL, typename T>
vec<COL, T> operator*(const mat<ROW, COL, T> &m1, const vec<COL, T> &v1) {
  vec<COL, T> res;

  for (size_t j = 0; j < COL; j++) {
    for (size_t i = 0; i < ROW; i++) {
      res[j] += m1[j][i] * v1[i];
    }
  }

  return res;
}

template<size_t ROW, size_t COL, typename T>
vec<COL, T> operator*(const vec<ROW, T> &v1, const mat<ROW, COL, T> &m1) {
  vec<COL, T> res;
  for (size_t i = 0; i < COL; i++) {
    for (size_t j = 0; j < ROW; j++) {
      res[i] += v1[j] * m1[j][i];
    }
  }

  return res;
}

template<typename T>
vec<3, T> cross(const vec<3, T> &v1, const vec<3, T> &v2) {
  vec<3, T> res;
  res[0] = v1[1] * v2[2] - v1[2] * v2[1];
  res[1] = v1[2] * v2[0] - v1[0] * v2[2];
  res[2] = v1[0] * v2[1] - v1[1] * v2[0];
  return res;
}

template<size_t U, size_t V, size_t W, typename T>
mat<U, W, T> operator*(const mat<U, V, T> &m1, const mat<V, W, T> &m2) {
  mat<U, W, T> res;
  for (size_t i = 0; i < U; i++) {
    for (size_t j = 0; j < V; j++) {
      for (size_t k = 0; k < W; k++) {
        res[i][j] += m1[i][k] * m2[k][j];
      }
    }
  }

  return res;
}

template<size_t DIMS, typename T>
vec<3, float> barycentric(const vec<DIMS, T> &a, const vec<DIMS, T> &b, const vec<DIMS, T> &c, const vec<DIMS, T> &p) {
  typedef vec<3, float> return_type;
  return_type res;
  if (DIMS < 2) {
    return return_type{1.f, 1.f, 1.f};
  } else {
    return_type s[2];
    for (int i = 0; i < 2; i++) {
      s[i][0] = c[i] - a[i];
      s[i][1] = b[i] - a[i];
      s[i][2] = a[i] - p[i];
    }

    return_type u = cross(s[0], s[1]);
    if (std::abs(u[2]) > 1e-2) {
      return return_type (1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]);
    } else {
      return return_type {-1, 1, 1};
    }
  }

  return res;
}

typedef vec<2, float> Vec2f;
typedef vec<3, float> Vec3f;
typedef vec<4, float> Vec4f;
typedef vec<2, int> Vec2i;
typedef vec<3, int> Vec3i;
typedef vec<4, int> Vec4i;
typedef mat<4, 4, float> Mat44;
}
