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
  std::cout << 1/dt << std::endl;
  if (is_key_pressed(VK_ESCAPE))
    schedule_quit_game();

  if (!started) {
    if (is_key_pressed(VK_RETURN))
      started = true;
    else
      return;
  }

  if ((world.player.lives == 0 || world.asteroids.empty())
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

  display::Color c{255, 200, 200};
  if (player.invincible) {
    if ((int)(world.time / 0.1) % 2 == 0)
      c = display::Color{0, 0, 255};
    else
      c = display::Color{0, 255, 255};
  }

  for (auto seg : lines) {
    display::line(c, w2s(player.body.trans.apply(seg.a)), w2s(player.body.trans.apply(seg.b)));
  }
}

void drawShoot(Transform trans) {
  display::circle(display::Color{30, 255, 255}, w2s(trans.pos), w2s(Projectile::radius));
}

void drawAsteroid(const Asteroid &ast) {\
  Vec p = w2s(ast.body.trans.pos);
  float r = w2s(ast.radius);

  int x0 = p.x - r;
  int y0 = p.y - r;
  int w = r * 2;
  int h = w;
  display::sprite(x0, y0, w, h, display::asteroid);
}

void draw()
{
  std::fill(&buffer[0][0], &buffer[0][0] + SCREEN_WIDTH * SCREEN_HEIGHT, 0);

  for (auto &asteroid : world.asteroids)
    drawAsteroid(asteroid);
  for (auto &shoot : world.projectiles)
    drawShoot(shoot.body.trans);

  if (world.player.lives > 0)
    drawPlayer(world.player);
  
  for (int i = 0; i < world.player.lives; i++) {
    display::sprite(50 + i * 100, 50, 80, 80, display::hearth);
  }

  {
    char score_str[6];
    std::snprintf(score_str, 6, "%+05d", world.player.score);
    if (world.player.score >= 0)
      score_str[0] = ' '; // remove minus sign, i don't know printf specifiers
    display::text(SCREEN_WIDTH, display::font_size, "Score: " + std::string(score_str) + " ", display::TextAlign::RIGHT);
  }

  if (world.player.lives == 0) {
    display::text(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, "Your ship has crashed!", display::TextAlign::CENTER);
    display::text(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, "Press ENTER to restart!", display::TextAlign::CENTER);
  }

  if (world.asteroids.size() == 0) {
    display::text(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, "Level cleared!", display::TextAlign::CENTER);
    display::text(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, "Press ENTER to restart!", display::TextAlign::CENTER);
  }

  if (!started) {
    display::text(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, "Press ENTER to start!", display::TextAlign::CENTER);
  }
}

void finalize()
{
}
