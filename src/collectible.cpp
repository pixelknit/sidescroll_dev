#include "collectible.hpp"

Collectible::Collectible(Vector2 pos, Texture2D sheet)
    : position(pos), radius(12), collected(false), anim({{0, 200, 16, 16}, 8}) {
} // Adjust frame in your sprite

void Collectible::Update(float deltaTime) {
  if (!collected)
    anim.Update(deltaTime);
}

void Collectible::Draw(Texture2D &spriteSheet) {
  if (collected)
    return;

  Rectangle source = anim.GetCurrentFrame();
  Rectangle dest = {position.x - radius, position.y - radius, radius * 2,
                    radius * 2};
  DrawTexturePro(spriteSheet, source, dest, {0, 0}, 0, WHITE);
}

bool Collectible::CheckCollision(const Rectangle &player) {
  if (collected)
    return false;
  Rectangle itemRect = {position.x - radius, position.y - radius, radius * 2,
                        radius * 2};
  return CheckCollisionRecs(player, itemRect);
}