#include "player.hpp"
#include "config.h"

Player::Player(Texture2D &spriteSheet)
    : position({100, 300}), velocity({0, 0}), width(32), height(48),
      facingRight(true), isGrounded(false), canAttack(true), attackTimer(0),
      state(IDLE), idleAnim({{0, 0, 32, 48}, 4}), // Adjust based on your sprite
      runAnim({{0, 48, 32, 48}, 6}),              // Next row
      jumpAnim({{0, 96, 32, 48}, 2}),             // Jump frames
      attackAnim({{0, 144, 48, 48}, 4})           // Attack frames (wider)
{
  currentAnim = &idleAnim;
}

void Player::Update(float deltaTime, const std::vector<Rectangle> &platforms) {
  // Input handling
  velocity.x = 0;

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

  // Apply gravity
  velocity.y += GRAVITY * deltaTime;

  // Update position
  position.x += velocity.x * deltaTime;
  position.y += velocity.y * deltaTime;

  // Platform collision
  isGrounded = false;
  Rectangle playerRect = GetBounds();

  for (const auto &platform : platforms) {
    if (CheckCollisionRecs(playerRect, platform)) {
      // Landing on top
      if (velocity.y > 0 &&
          playerRect.y + playerRect.height - velocity.y * deltaTime <=
              platform.y) {
        position.y = platform.y - height;
        velocity.y = 0;
        isGrounded = true;
        if (state == JUMPING)
          state = IDLE;
      }
      // Hitting head
      else if (velocity.y < 0 && playerRect.y - velocity.y * deltaTime >=
                                     platform.y + platform.height) {
        position.y = platform.y + platform.height;
        velocity.y = 0;
      }
      // Side collisions
      else if (velocity.x > 0) {
        position.x = platform.x - width;
      } else if (velocity.x < 0) {
        position.x = platform.x + platform.width;
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

  // Debug hitbox (remove in final)
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