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
  static constexpr float life_duration = 1;
  static constexpr float radius = 0.1;
};

struct Player {
  Body body;
  float last_shoot_t = 0;
  bool invincible = false;
  float invincible_start = 0;
  int score = 0;
  int lifes = 3;

  // Player(Body body): body(body) {}

  constexpr static float shoot_delay = 0.02;
  constexpr static float radius = 1;
  constexpr static float invincible_dur = 1;
};

struct Asteroid {
  Body body;

  float radius;

  bool alive = true;
  Vec kill_dir;

  static constexpr float maxSpeed = 3;
  static constexpr float minRadius = 1.5;
};

struct World {
  Vec size;
  Player player;
  std::vector<Asteroid> asteroids;
  std::vector<Projectile> projectiles;
  float time = 0;

  void shoot() {
    if (time < player.last_shoot_t + player.shoot_delay)
      return;
    player.last_shoot_t = time;

    Transform trans = player.body.trans;
    Vec vel = Vec{Projectile::speed, 0}.rotate(trans.rot) + player.body.vel;
    projectiles.push_back(
      Projectile{Body{trans, vel}, time}
    );
  }

  void wrap(Body &body) {
    body.trans.pos = body.trans.pos.wrap(size);
  }

  void spawnRandomAsteroid() {
    auto frand = []() { return (rand() % 1000) / 1000.0f; };

    float r = 4; //frand() * 5 + 5;
    
    Vec pos{frand() * size.x, frand() * size.y};
    Vec vel = Vec{frand(), 0} * Asteroid::maxSpeed;
    float rot = frand() * 3.1415;
    vel = vel.rotate(rot);
    asteroids.push_back(Asteroid{Body{{pos, rot}, vel}, r});
  }

  void step(float dt, int steer, int move) {
    time += dt;
    player.body.vel += dt * Vec{1, 0}.rotate(player.body.trans.rot) * 30 * move;
    player.body.trans.rot += dt * steer * 5;

    // Check projectiles for exiration

    for (Projectile &proj : projectiles) {
      if (time - proj.spawn_time > proj.life_duration) {
        proj.alive = false;
      }
    }

    // Player-asteroid collision

    if (!player.invincible) {
      for (Asteroid &asteroid : asteroids) {
        if (!asteroid.alive) continue;
        if ((player.body.trans.pos - asteroid.body.trans.pos).len() <= asteroid.radius + player.radius) {
          player.invincible = true;
          player.invincible_start = time;
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
          asteroid.kill_dir = proj.body.vel;
          break;
        }
      }
    }

    // Split damaged asteroids 

    std::vector<Asteroid> new_ast;
    for (Asteroid &asteroid : asteroids) {
      if (asteroid.alive) continue;
      if (asteroid.radius < Asteroid::minRadius) continue;
      Body b1 = asteroid.body, b2 = asteroid.body;
      Vec right = asteroid.kill_dir.normalized().rotate(-pi / 2);

      b1.trans.pos += right * asteroid.radius / 2;
      b2.trans.pos -= right * asteroid.radius / 2;
      b1.vel += right * asteroid.body.vel.len() / 2; // haha energy presetvation
      b2.vel -= right * asteroid.body.vel.len() / 2;

      new_ast.push_back(Asteroid{b1, asteroid.radius / 1.7f});
      new_ast.push_back(Asteroid{b2, asteroid.radius / 1.7f});
    }

    asteroids.insert(asteroids.end(), new_ast.begin(), new_ast.end());

    // Remove dead asteroids

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

    // Phystics move step

    player.body.step(dt), wrap(player.body);
    for (Asteroid &asteroid : asteroids)
      asteroid.body.step(dt), wrap(asteroid.body);
    for (Projectile &projectile : projectiles)
      projectile.body.step(dt), wrap(projectile.body);
  }
};
