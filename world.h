#pragma once

#include <memory.h>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <deque>
#include <cmath>
#include <vector>
#include <array>

#include "geometry.h"
#include "Engine.h"


const double pi = std::acos(-1);

struct Transform {
  Vec pos; float rot;

  Vec getDir() {
    return Vec{1, 0}.rotate(rot);
  }

  Vec apply(Vec x) const {
    return pos + x.rotate(rot);
  }
};

struct Body {
  Transform trans;
  Vec vel;

  void step(float dt) {
    trans.pos += vel * dt;
  }
};

struct Projectile {
  Body body;
  float spawn_time;
  bool alive = true;

  static constexpr float speed = 100;
  static constexpr float life_duration = 0.75;
  static constexpr float radius = 0.1;
};

struct Player {
  Body body;
  float last_shoot_t = 0;
  bool invincible = false;
  float invincible_start = 0;
  int score = 0;
  int lives = 3;

  bool alive() { return lives > 0; }

  constexpr static float shoot_delay = 0.1;
  constexpr static float radius = 1;
  constexpr static float invincible_dur = 1;
  constexpr static float turn_speed = 3;
  constexpr static float acceleration = 30;
};

struct Asteroid {
  Body body;
  float radius;
  int color; // 0, 1, 2

  bool alive = true;
  Vec hit_dir;

  static constexpr float max_speed = 3;
  static constexpr float min_radius = 1.5;
};

struct Input {
  int move; // -1, 0, 1 - back, nothing, forward
  int steer; // -1, 0, 1 - left, nothing, right
  bool shoot;
};

struct World {
  Vec size;
  Player player;
  std::vector<Asteroid> asteroids;
  std::vector<Projectile> projectiles;
  float time = 0;

  World(Vec size_): size(size_) {
    resetPlayerPos();
    for (int i = 0; i < 10; i++)
      spawnRandomAsteroid();
  }


  void resetPlayerPos() {
    player.body.trans.pos = size / 2.0f;
    player.body.trans.rot = pi/2;
    player.body.vel = Vec{0, 0};
  }

  void shootIfReady() {
    if (time < player.last_shoot_t + player.shoot_delay)
      return;
    player.last_shoot_t = time;

    Vec vel = player.body.trans.getDir() * Projectile::speed + player.body.vel;
    projectiles.push_back(
      Projectile{Body{player.body.trans, vel}, time}
    );
  }

  void wrap(Body &body) {
    body.trans.pos = body.trans.pos.wrap(size);
  }

  void spawnRandomAsteroid() {
    auto frand = []() { return (rand() % 1000) / 1000.0f; };

    float r = frand() * 3 + 5;
    int color = rand() % 3;

    Vec pos;
    do {
      pos = Vec{frand() * size.x, frand() * size.y};
    } while ((pos - player.body.trans.pos).len() < r * 2 + player.radius);
    
    Vec vel = Vec{1, 0} * (frand() * 0.5 + 1) * Asteroid::max_speed;
    float rot = frand() * 3.1415;
    vel = vel.rotate(rot);
    asteroids.push_back(Asteroid{Body{{pos, rot}, vel}, r, color});
  }

  void step(float dt, Input inp) {
    time += dt;

    // Player control

    if (player.alive()) {
      if (inp.shoot)
        shootIfReady();
      player.body.vel += dt * inp.move * Player::acceleration * player.body.trans.getDir();
      player.body.trans.rot += dt * inp.steer * Player::turn_speed;
    }

    // Check projectiles for exiration

    for (Projectile &proj : projectiles) {
      if (time - proj.spawn_time > proj.life_duration) {
        proj.alive = false;
      }
    }

    // Player-asteroid collision

    if (player.lives == 0) {
      // pass
    } else if (!player.invincible) {
      for (Asteroid &asteroid : asteroids) {
        if (!asteroid.alive) continue;

        if ((player.body.trans.pos - asteroid.body.trans.pos).len() <= asteroid.radius + player.radius) {
          player.invincible = true;
          player.invincible_start = time;
          player.lives--;
          player.score -= 100;
          resetPlayerPos();
          break;
        }
      }
    } else if (time - player.invincible_start >= player.invincible_dur) {
      player.invincible = false;
    }

    // Asteroid-projectile collisions

    for (Projectile &proj : projectiles) {
      if (!proj.alive) continue;

      for (Asteroid &asteroid : asteroids) {
        if (!asteroid.alive) continue;

        if ((proj.body.trans.pos - asteroid.body.trans.pos).len() <= asteroid.radius + Projectile::radius) {
          proj.alive = false;
          asteroid.alive = false;
          asteroid.hit_dir = proj.body.vel;
          player.score += 10;
          break;
        }
      }
    }

    // Split damaged asteroids 

    std::vector<Asteroid> debris;
    for (Asteroid &asteroid : asteroids) {
      if (asteroid.alive) continue;
      if (asteroid.radius / 1.7f < Asteroid::min_radius) continue;

      Body b1 = asteroid.body, b2 = asteroid.body;
      Vec right = asteroid.hit_dir.normalized().rotate(-pi / 2);

      b1.trans.pos += right * asteroid.radius / 2;
      b2.trans.pos -= right * asteroid.radius / 2;
      b1.vel += right * asteroid.body.vel.len() * 2; // haha no energy presetvation
      b2.vel -= right * asteroid.body.vel.len() * 2;

      debris.push_back(Asteroid{b1, asteroid.radius / 1.7f, asteroid.color});
      debris.push_back(Asteroid{b2, asteroid.radius / 1.7f, asteroid.color});
    }

    asteroids.insert(asteroids.end(), debris.begin(), debris.end());

    // Remove dead asteroids and projectiles

    {
      auto new_end = std::remove_if(asteroids.begin(), asteroids.end(), [](Asteroid &ast) {
        return !ast.alive;
      });
      asteroids.erase(new_end, asteroids.end());
    }
    {
      auto new_end = std::remove_if(projectiles.begin(), projectiles.end(), [](Projectile &proj) {
        return !proj.alive;
      });
      projectiles.erase(new_end, projectiles.end());
    }

    // Physics move step

    if (player.alive())
      player.body.step(dt), wrap(player.body);
    for (Asteroid &asteroid : asteroids)
      asteroid.body.step(dt), wrap(asteroid.body);
    for (Projectile &projectile : projectiles)
      projectile.body.step(dt), wrap(projectile.body);
  }
};
