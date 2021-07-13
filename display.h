#pragma once

#include <cassert>
#include <utility>
#include <algorithm>
#include <string>

#include "Engine.h"
#include "geometry.h"

namespace display {
  struct Color {
    uint8_t b, g, r, o; // yes, o stands for opacity, treated binary
  };

  namespace colors {
    constexpr Color sexyRed{0x36, 0x43, 0xf4};
  }

  inline bool onScreen(int x, int y) {
    return (
         0 <= x && x < SCREEN_WIDTH
      && 0 <= y && y < SCREEN_HEIGHT
    );
  }

  inline Color& at(int x, int y) {
    assert(onScreen(x, y));
    return reinterpret_cast<Color&>(buffer[y][x]);
  }

  inline int clip_x(int x) { return std::max(0, std::min(x, SCREEN_WIDTH  - 1)); }
  inline int clip_y(int y) { return std::max(0, std::min(y, SCREEN_HEIGHT - 1)); }

  inline void rect(Color color, int x0, int y0, int w, int h) {
    if (x0 + w <= 0 || x0 >= SCREEN_WIDTH) return;
    if (y0 + h <= 0 || y0 >= SCREEN_HEIGHT) return;

    int x1 = x0 + w - 1, y1 = y0 + h - 1;
    x0 = clip_x(x0), x1 = clip_x(x1), y0 = clip_y(y0), y1 = clip_y(y1);

    for (int y = y0; y <= y1; y++)
      std::fill(&buffer[y][x0], &buffer[y][x1] + 1,
                reinterpret_cast<uint32_t&>(color));
  }

  inline void rect(Color color, float x0, float y0, float w, float h) {
    rect(color, (int)std::round(x0), (int)std::round(y0), (int)std::round(w), (int)std::round(h));
  }

  inline void line(Color color, Vec p1, Vec p2) {
    using std::abs, std::round, std::swap;
    if (abs(p1.x - p2.x) >= abs(p1.y - p2.y)) {
      if (p1.x > p2.x)
        swap(p1, p2);
      for (int x = round(p1.x); x <= round(p2.x); x++) {
        int y = round(p1.y + (x - p1.x) / (p2.x - p1.x) * (p2.y - p1.y));
        if (onScreen(x, y))
          at(x, y) = color;
      }
    } else {
      if (p1.y > p2.y)
        swap(p1, p2);
      for (int y = round(p1.y); y <= round(p2.y); y++) {
        int x = round(p1.x + (y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x));
        if (onScreen(x, y))
          at(x, y) = color;
      }
    }
  }

  inline void circle(Color color, Vec p, float r) {
    using std::abs, std::round;

    int y0 = clip_y(std::floor(p.y - r));
    int y1 = clip_y(std::ceil(p.y + r));

    int x0 = std::round(p.x), x1 = std::round(p.x);
    for (int y = y0; y <= y1; y++) {
      auto isInside = [y, r, p](int x) {
        return (Vec{(float)x, (float)y} - p).len() <= r;
      };

      while (!isInside(x0 + 1) && x0 + 1 < p.x) x0++;
      while (isInside(x0)) x0--;

      while (!isInside(x1 - 1) && x1 - 1 > p.x) x1--;
      while (isInside(x1)) x1++;

      if (x1 < 0 || x0 >= SCREEN_WIDTH) continue;

      std::fill(&buffer[y][clip_x(x0)], &buffer[y][clip_x(x1)] + 1,
          reinterpret_cast<uint32_t&>(color));
    }
  }

  struct Sprite {
    static Sprite fromString(std::string s, std::vector<Color> pallete);

    std::vector<std::vector<Color>> grid;

    int width()  const { return grid.front().size(); }
    int height() const { return grid.size(); }
    Color at(int x, int y) const { return grid[y][x]; }
  };

  namespace sprites {
    extern Sprite hearth;
    extern Sprite asteroids[3];
    extern Sprite ascii[128];
    constexpr int font_size = 32;
  }

  inline void sprite(int x0, int y0, int w, int h, const Sprite &sprite) {
    for (int y = y0; y < y0 + h; y++)
    for (int x = x0; x < x0 + w; x++) {
      if (!onScreen(x, y)) continue;
      int sx = std::round((float)(x - x0) / (w - 1) * (sprite.width() - 1));
      int sy = std::round((float)(y - y0) / (h - 1) * (sprite.height() - 1));
      assert(sx < sprite.width());
      assert(sy < sprite.height());
      if (sprite.at(sx, sy).o == 0)
        at(x, y) = sprite.at(sx, sy);
    }
  }

  enum class TextAlign {LEFT, CENTER, RIGHT};

  inline void text(int x0, int y0, std::string s, TextAlign align = TextAlign::LEFT) {
    if (align != TextAlign::LEFT) {
      // I'm too lazy to align multiline strings
      assert(s.find('\n') == std::string::npos);
    }

    if (align == TextAlign::CENTER) {
      x0 -= s.size() * sprites::font_size / 2;
    } else if (align == TextAlign::RIGHT) {
      x0 -= s.size() * sprites::font_size;
    }

    int x = x0, y = y0;

    for (char c : s) {
      if (c == '\n') {
        y += sprites::font_size;
        x = x0;
      } else {
        sprite(x, y, sprites::font_size, sprites::font_size, sprites::ascii[c]);
        x += sprites::font_size;
      }
    }
  }
};