#pragma once

#include "raylib.h"
#include "config.h"
#include "player.hpp"
#include <vector>

struct Door {
  Vector2 position;
  float width, height;
  bool isOpen;
  bool isLocked;
  int requiredKeys;

  Door(float x, float y, int keysNeeded);
  void Update(Player &player, int playerKeys);
  void Draw(Texture2D &envSheet);
};