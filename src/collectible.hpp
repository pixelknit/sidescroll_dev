#pragma once

#include "animation.hpp"
#include "config.h"
#include "player.hpp"
#include "raylib.h"
#include <cmath>
#include <vector>

struct Collectible {
  Vector2 position;
  float radius;
  bool collected;
  Animation anim;

  Collectible(Vector2 pos, Texture2D sheet);
  void Update(float deltaTime);
  void Draw(Texture2D &spriteSheet);
  bool CheckCollision(const Rectangle &player);
};