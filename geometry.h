#pragma once

#include <cmath>

struct Vec {
  float x, y;

  friend Vec operator-(Vec v) { return {-v.x, -v.y}; }

  friend Vec operator+(Vec a, Vec b) { return {a.x + b.x, a.y + b.y}; }
  friend Vec operator-(Vec a, Vec b) { return {a.x - b.x, a.y - b.y}; }
  friend Vec operator/(Vec a, Vec b) { return {a.x / b.x, a.y / b.y}; }
  friend Vec operator*(Vec a, Vec b) { return {a.x * b.x, a.y * b.y}; }

  friend Vec operator*(Vec v, float k) { return {v.x * k, v.y * k}; }
  friend Vec operator*(float k, Vec v) { return {v.x * k, v.y * k}; }
  friend Vec operator/(Vec v, float k) { return {v.x / k, v.y / k}; }

  Vec& operator+=(Vec d)   { *this = *this + d; return *this; }
  Vec& operator-=(Vec d)   { *this = *this - d; return *this; }
  Vec& operator*=(float k) { *this = *this * k; return *this; }
  Vec& operator/=(float k) { *this = *this / k; return *this; }

  Vec rotate(float angle) const {
    using std::sin, std::cos;
    return Vec{
      cos(angle) * x - sin(angle) * y,
      sin(angle) * x + cos(angle) * y,
    };
  }

  Vec normalized() const { return (*this) / len(); }

  static float fmod(float x, float m) {
    if (x < 0) return m - std::fmod(-x, m);
    else       return std::fmod(x, m);
  }

  const Vec wrap(Vec bounds) const {
    return {fmod(x, bounds.x), fmod(y, bounds.y)};
  }

  const float len() const { return std::hypot(x, y); }
};
