#include "door.hpp"

Door::Door(float x, float y, int keysNeeded = 0)
    : position({x, y}), width(48), height(64), isOpen(false),
      isLocked(keysNeeded > 0), requiredKeys(keysNeeded) {}

void Door::Update(Player &player, int playerKeys) {
  Rectangle doorRect = {position.x, position.y, width, height};
  Rectangle playerRect = player.GetBounds();

  if (CheckCollisionRecs(playerRect, doorRect) && !isOpen) {
    if (!isLocked || playerKeys >= requiredKeys) {
      if (IsKeyPressed(KEY_E)) {
        isOpen = true;
      }
    }
  }
}

void Door::Draw(Texture2D &envSheet) {
  // Door frames in environment sprite (adjust coordinates)
  Rectangle source;
  if (isOpen) {
    source = {96, 0, 48, 64}; // Open door frame
  } else if (isLocked) {
    source = {48, 0, 48, 64}; // Locked door frame
  } else {
    source = {0, 0, 48, 64}; // Closed door frame
  }

  Rectangle dest = {position.x, position.y, width, height};
  DrawTexturePro(envSheet, source, dest, {0, 0}, 0, WHITE);

  // Interaction prompt
  if (!isOpen) {
    DrawText("Press E", position.x - 10, position.y - 20, 10, WHITE);
  }
}