#include "enemy.hpp"

Enemy::Enemy(float x, float y, float patrolDist = 100)
    : position({x, y}), startPos({x, y}), patrolDistance(patrolDist), speed(50),
      width(32), height(48), alive(true), health(2) {}

void Enemy::Update(float deltaTime, Player &player) {
  if (!alive)
    return;

  // Simple patrol AI
  float offset = sin(GetTime() * speed * 0.01) * patrolDistance;
  position.x = startPos.x + offset;

  // Check if hit by player attack
  if (player.state == ATTACKING &&
      CheckCollisionRecs(GetBounds(), player.GetAttackHitbox())) {
    health--;
    if (health <= 0)
      alive = false;
  }

  // Damage player on contact
  if (CheckCollisionRecs(GetBounds(), player.GetBounds())) {
    // Push player back
    if (player.position.x < position.x) {
      player.position.x -= 50;
    } else {
      player.position.x += 50;
    }
  }
}

void Enemy::Draw(Texture2D &spriteSheet) {
  if (!alive)
    return;

  // Use enemy frames from character sheet or separate enemy sprite
  Rectangle source = {0, 192, 32, 48}; // Adjust based on your sprite
  Rectangle dest = {position.x, position.y, width, height};
  DrawTexturePro(spriteSheet, source, dest, {0, 0}, 0, WHITE);
}

Rectangle Enemy::GetBounds() const {
  return {position.x, position.y, width, height};
}