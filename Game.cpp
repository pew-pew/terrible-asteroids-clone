#include "Engine.h"

#include <memory.h>
#include <cassert>
#include <iostream>
#include <algorithm>
#include <deque>
#include <cmath>

#include "geometry.h"
#include "display.h"
#include "world.h"

World world;

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
  world.size = Vec{100, 100.0f * SCREEN_HEIGHT / SCREEN_WIDTH};
  world.player.body.trans.pos = world.size / 2.0f;
  world.spawnRandomAsteroid();
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

  float t_bef = world.time;
  float t_aft = world.time + dt;

  if (round(t_bef * 5) != round(t_aft * 5)) {
    world.spawnRandomAsteroid();
  }

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
  display::circle(display::Color{0, 0, 255}, w2s(ast.body.trans.pos), w2s(ast.radius * 0.9));
}

void draw()
{
  std::fill(&buffer[0][0], &buffer[0][0] + SCREEN_WIDTH * SCREEN_HEIGHT, 0);

  for (auto &asteroid : world.asteroids)
    drawAsteroid(asteroid);
  for (auto &shoot : world.projectiles)
    drawShoot(shoot.body.trans);
  drawPlayer(world.player);
}

void finalize()
{
}
