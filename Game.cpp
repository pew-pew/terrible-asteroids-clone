#include "Engine.h"

#include <memory.h>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <deque>
#include <cmath>
#include <optional>

#include "geometry.h"
#include "display.h"
#include "world.h"

const Vec world_size = Vec{100, 100.0f * SCREEN_HEIGHT / SCREEN_WIDTH};
World world(world_size);
bool started = false;

Vec w2s(Vec v) {
  Vec screen_size{SCREEN_WIDTH, SCREEN_HEIGHT};
  v.y = world.size.y - v.y;
  return v / world.size * screen_size;
}

float w2s(float x) {
  Vec screen_size{SCREEN_WIDTH, SCREEN_HEIGHT};
  return x / world.size.x * screen_size.x;
}


void initialize()
{
}

void handleControls(int &rot, int& acc) {
  rot = 0;
  acc = 0;
  if (is_key_pressed(VK_UP))    acc += 1;
  if (is_key_pressed(VK_DOWN))  acc -= 1;
  if (is_key_pressed(VK_LEFT))  rot += 1;
  if (is_key_pressed(VK_RIGHT)) rot -= 1;
}

void act(float dt)
{
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();

  if (!started) {
    if (is_key_pressed(VK_RETURN))
      started = true;
    else
      return;
  }

  if ((world.player.lifes == 0 || world.asteroids.empty())
      && is_key_pressed(VK_RETURN))
  {
    world = World(world_size);
  }

  // float t_bef = world.time;
  // float t_aft = world.time + dt;

  // if (round(t_bef * 5) != round(t_aft * 5)) {
  //   world.spawnRandomAsteroid();
  // }

  if (is_key_pressed(VK_SPACE))
    world.shoot();

  int steer, move;
  handleControls(steer, move);
  world.step(dt, steer, move);
}

void drawPlayer(const Player &player) {
  struct Seg{Vec a, b;} lines[] = {
    {{ 1,  0}, {-1,  1}},
    {{ 1,  0}, {-1, -1}},
    {{-1, -1}, {-1,  1}}
  };

  Vec screen_size{SCREEN_WIDTH, SCREEN_HEIGHT};

  display::Color c{255, 0, 255};
  if (player.invincible) {
    c = display::Color{0, 0, 255};
  }
  for (auto seg : lines) {
    display::line(c, w2s(player.body.trans.apply(seg.a)), w2s(player.body.trans.apply(seg.b)));
  }
}

void drawShoot(Transform trans) {
  display::circle(display::Color{0, 0, 255}, w2s(trans.pos), w2s(Projectile::radius));
}

void drawAsteroid(const Asteroid &ast) {
  display::circle(display::Color{0, 125, 255}, w2s(ast.body.trans.pos), w2s(ast.radius));
  display::circle(display::Color{0, 100, 200}, w2s(ast.body.trans.pos), w2s(ast.radius * 0.9));
}

void drawHearth(Vec pos) {
  display::circle(display::Color{30, 30, 255}, pos + Vec{-10, -10}, 30);
  display::circle(display::Color{30, 30, 255}, pos + Vec{+10, -10}, 30);
  display::circle(display::Color{30, 30, 255}, pos + Vec{  0, +10}, 30);
}

void draw()
{
  std::fill(&buffer[0][0], &buffer[0][0] + SCREEN_WIDTH * SCREEN_HEIGHT, 0);

  for (auto &asteroid : world.asteroids)
    drawAsteroid(asteroid);
  for (auto &shoot : world.projectiles)
    drawShoot(shoot.body.trans);

  if (world.player.lifes > 0)
    drawPlayer(world.player);
  
  for (int i = 0; i < world.player.lifes; i++) {
    drawHearth(Vec{100 + i * 100, 100});
  }

  if (world.player.lifes == 0) {
    for (int i = 0; i < 6; i++)
      display::rect(display::Color{200, 200, 200}, 200 + 100 * i, SCREEN_HEIGHT / 2, 70, 70);
  }

  if (world.asteroids.size() == 0) {
    for (int i = 0; i < 6; i++)
      display::rect(display::Color{200, 200, 0}, 200 + 100 * i, SCREEN_HEIGHT / 2, 70, 70);
  }

  if (!started) {
    for (int i = 0; i < 6; i++)
      display::rect(display::Color{0, 200, 200}, 200 + 100 * i, SCREEN_HEIGHT / 2, 70, 70);
  }
}

void finalize()
{
}
