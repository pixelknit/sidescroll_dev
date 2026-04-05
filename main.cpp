#include "raylib.h"
#include <cmath>
#include <vector>
#include "src/config.h"
#include "src/animation.hpp"
#include "src/player.hpp"
#include "src/collectible.hpp"
#include "src/door.hpp" 
#include "src/enemy.hpp"
#include "src/parallax.h"
#include "src/game.hpp"


int main() {
  Game game;

  while (!game.ShouldClose()) {
    if (game.state == GAME_OVER)
      return 0;

    float deltaTime = GetFrameTime();

    game.HandleInput();
    game.Update(deltaTime);
    game.Draw();
  }

  return 0;
}