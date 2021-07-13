#pragma once

#include <random>

#include "geometry.h"
#include "display.h"

// Just for 
struct StarrySky {
  struct Star {
    Vec pos;
    float size;
    float spawn_time;
    float duration;
  };

  std::vector<Star> stars;
  float time;
  std::mt19937 rng;
  static constexpr int stars_per_second = 3;

  Star makeStar() {
    Vec pos = Vec{
      std::uniform_real_distribution<>(0, SCREEN_WIDTH - 1)(rng),
      std::uniform_real_distribution<>(0, SCREEN_HEIGHT - 1)(rng)
    };
    float size = std::uniform_real_distribution<>(1, 10)(rng);
    float dur = std::uniform_real_distribution<>(1, 5)(rng);
    return Star{pos, size, time, dur};
  }

  void act(float dt) {
    time += dt;
    // Clear old stars
    auto end = std::remove_if(stars.begin(), stars.end(), [&](Star &star) {
      return star.spawn_time + star.duration < time;
    });
    stars.erase(end, stars.end());

    int new_stars = std::poisson_distribution<>(dt * stars_per_second)(rng);
    for (int i = 0; i < new_stars; i++)
      stars.push_back(makeStar());
  }

  void drawStar(const Star &star) {
    float alive_frac = (time - star.spawn_time) / star.duration;
    float intensity = 1 - std::abs(alive_frac - 0.5) * 2;
    float real_sz = star.size * intensity;
    display::Color color{100, 100, 100};
    display::line(color, star.pos + Vec{-1,  0} * real_sz, star.pos + Vec{+1,  0} * real_sz);
    display::line(color, star.pos + Vec{ 0, -1} * real_sz, star.pos + Vec{ 0, +1} * real_sz);
  }

  void draw() {
    for (const Star &star : stars)
      drawStar(star);
  }
};