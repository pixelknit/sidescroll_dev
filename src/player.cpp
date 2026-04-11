#include "player.hpp"
#include "config.h"

Player::Player(Texture2D &spriteSheet)
    : position({100, 300}), velocity({0, 0}), width(80), height(80),
      facingRight(true), isGrounded(false), alive(true), health(100),
      canAttack(true), attackTimer(0),
      state(IDLE), idleAnim({{0, 0, PLAYER_SIZE, PLAYER_SIZE}, 4}),
      runAnim({{0, PLAYER_SIZE, PLAYER_SIZE, PLAYER_SIZE}, 6}),
      jumpAnim({{0, PLAYER_SIZE * 2, PLAYER_SIZE, PLAYER_SIZE}, 11}),
      attackAnim({{0, PLAYER_SIZE * 3, PLAYER_SIZE, PLAYER_SIZE}, 4}) {
  currentAnim = &idleAnim;
}

void Player::Update(float deltaTime, const std::vector<Rectangle> &platforms) {
  // Input handling
  velocity.x = 0;

  if (health <= 0){
    alive = false;
  }

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
    velocity.x = -PLAYER_SPEED;
    facingRight = false;
    if (isGrounded && state != ATTACKING)
      state = RUNNING;
  }
  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
    velocity.x = PLAYER_SPEED;
    facingRight = true;
    if (isGrounded && state != ATTACKING)
      state = RUNNING;
  }

  // Jump
  if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W)) && isGrounded &&
      state != ATTACKING) {
    velocity.y = JUMP_FORCE;
    isGrounded = false;
    state = JUMPING;
  }

  // Attack
  if (IsKeyDown(KEY_J) && canAttack) {
    state = ATTACKING;
    canAttack = false;
    attackTimer = 0.4f; // Attack duration
  }

  if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_D) &&
      !IsKeyDown(KEY_J) && !IsKeyDown(KEY_SPACE)) {
    state = IDLE;
  }

  if (!isGrounded) {
    velocity.y += GRAVITY * deltaTime;
  }

  // Store previous position for collision resolution
  Vector2 prevPos = position;

  // Update X position first (separate axis movement)
  position.x += velocity.x * deltaTime;

  // Check X collisions with platforms
  Rectangle playerRectX = {position.x, prevPos.y, width, height};
  for (const auto &plat : platforms) {
    if (CheckCollisionRecs(playerRectX, plat)) {
      if (velocity.x > 0)
        position.x = plat.x - width;
      else if (velocity.x < 0)
        position.x = plat.x + plat.width;
      velocity.x = 0;
    }
  }

  // Update Y position
  position.y += velocity.y * deltaTime;
  isGrounded = false;

  // Check Y collisions
  Rectangle playerRectY = {position.x, position.y, width, height};
  for (const auto &plat : platforms) {
    if (CheckCollisionRecs(playerRectY, plat)) {
      // Landing on top
      if (velocity.y > 0 && prevPos.y + height <= plat.y + 5) { // +5 tolerance
        position.y = plat.y - height;
        velocity.y = 0;
        isGrounded = true;
      }
      // Hitting head
      else if (velocity.y < 0 && prevPos.y >= plat.y + plat.height - 5) {
        position.y = plat.y + plat.height;
        velocity.y = 0;
      }
    }
  }

  // Screen boundaries
  if (position.x < 0)
    position.x = 0;
  if (position.x > 5000)
    position.x = 5000; // Level width limit

  // Attack timer
  if (!canAttack) {
    attackTimer -= deltaTime;
    if (attackTimer <= 0) {
      canAttack = true;
      if (isGrounded)
        state = IDLE;
      else
        state = JUMPING;
    }
  }

  // Set animation based on state
  switch (state) {
  case IDLE:
    currentAnim = &idleAnim;
    break;
  case RUNNING:
    currentAnim = &runAnim;
    break;
  case JUMPING:
    currentAnim = &jumpAnim;
    break;
  case ATTACKING:
    currentAnim = &attackAnim;
    break;
  }

  currentAnim->Update(deltaTime);
}

void Player::Draw(Texture2D &spriteSheet) {
  Rectangle source = currentAnim->GetCurrentFrame();

  // Flip if facing left
  if (!facingRight)
    source.width = -source.width;

  Rectangle dest = {position.x, position.y, width, height};
  Vector2 origin = {0, 0};

  DrawTexturePro(spriteSheet, source, dest, origin, 0.0f, WHITE);

  // Debug hitbox
  // DrawRectangleLines(position.x, position.y, width, height, RED);
}

Rectangle Player::GetBounds() const {
  return {position.x, position.y, width, height};
}

Rectangle Player::GetAttackHitbox() const {
  if (state != ATTACKING)
    return {0, 0, 0, 0};

  float attackRange = 30;
  if (facingRight) {
    return {position.x + width, position.y + 10, attackRange, height - 20};
  } else {
    return {position.x - attackRange, position.y + 10, attackRange,
            height - 20};
  }
}
