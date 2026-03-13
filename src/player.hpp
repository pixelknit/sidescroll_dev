#pragma once

#include "raylib.h"
#include "config.h"
#include "animation.hpp"
#include <cmath>
#include <vector>

struct Player {
  Vector2 position;
  Vector2 velocity;
  float width, height;
  bool facingRight;
  bool isGrounded;
  bool canAttack;
  float attackTimer;

  PlayerState state;
  Animation *currentAnim;
  Animation idleAnim;
  Animation runAnim;
  Animation jumpAnim;
  Animation attackAnim;

  Player(Texture2D &spriteSheet);

  void Update(float deltaTime, const std::vector<Rectangle> &platforms); 

  void Draw(Texture2D &spriteSheet); 

  Rectangle GetBounds() const;

  Rectangle GetAttackHitbox() const;

};