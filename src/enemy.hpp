#pragma once

#include "raylib.h"
#include "config.h"
#include "animation.hpp"
#include "player.hpp"
#include <cmath>
#include <vector>

struct Enemy {
  Vector2 position;
  Vector2 startPos;
  float patrolDistance;
  float speed;
  float width, height;
  bool alive;
  int health;

  Enemy(float x, float y, float patrolDist);

  void Update(float deltaTime, Player &player);

  void Draw(Texture2D &spriteSheet);

  Rectangle GetBounds() const;
};