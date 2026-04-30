#include "game.hpp"
#include "config.h"
#include <iostream>

Game::Game() : player(characterSheet), score(0), keys(0), state(PLAYING) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Side Scroller - Raylib");
  SetTargetFPS(60);

  characterSheet = LoadTexture("assets/character_sprite.png");
  environmentSheet = LoadTexture("assets/environment_sprite.png");

  bgLayers = {
      {LoadTexture("assets/bg_05.png"), BG_LAYER_PARALLAX_MOTION_SPEED[0], 0},
      {LoadTexture("assets/bg_04.png"), BG_LAYER_PARALLAX_MOTION_SPEED[1], 0},
      {LoadTexture("assets/bg_03.png"), BG_LAYER_PARALLAX_MOTION_SPEED[2], 0},
      {LoadTexture("assets/bg_02.png"), BG_LAYER_PARALLAX_MOTION_SPEED[3], 0},
      {LoadTexture("assets/bg_01.png"), BG_LAYER_PARALLAX_MOTION_SPEED[4], 0},
  };

  fgLayers = {
      {LoadTexture("assets/front_01.png"), 2.5f, 0},
  };

  // Setup camera
  camera.target = {0, 0};
  camera.offset = {SCREEN_WIDTH / 3.0f, SCREEN_HEIGHT / 3.0f};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f;

  CreateLevel();
}

void Game::CreateLevel() {
  // Ground platforms
  platforms.push_back({0, 600, 800, 120});
  platforms.push_back({900, 550, 400, 120});
  platforms.push_back({1400, 500, 300, 120});
  platforms.push_back({1800, 450, 200, 20});
  platforms.push_back({2100, 400, 200, 20});
  platforms.push_back({2400, 350, 500, 120});

  // Floating platforms
  platforms.push_back({300, 450, 100, 20});
  platforms.push_back({500, 360, 100, 20});
  platforms.push_back({700, 270, 100, 20});

  // Collectibles
  coins.emplace_back(Vector2{370, 410});
  coins.emplace_back(Vector2{550, 320});
  coins.emplace_back(Vector2{750, 220});
  coins.emplace_back(Vector2{1000, 500});
  coins.emplace_back(Vector2{1500, 450});

  // Doors
  doors.emplace_back(750.0f, 536.0f, 0);  // No key needed
  doors.emplace_back(2800.0f, 286.0f, 1); // Needs 1 key
  doors.emplace_back(310.0f, 355.0f, 0);  // Door to travel to

  // Enemies
  enemies.emplace_back(Enemy{1000, 475, 150});
  enemies.emplace_back(Enemy{1600, 422, 100});

  // Key pickup
  coins.emplace_back(Vector2{2200, 350});
}

void Game::Update(float deltaTime) {
  if (!player.alive){
    std::cout << "YOU HAVE DIED!\n";
    state = GAME_OVER;
  }

  if (state != PLAYING)
    return;

  // Update player
  player.Update(deltaTime, platforms);

  // Update BG
  //  UpdateParallax(player.position.x);

  // Update camera to follow player
  camera.target = {player.position.x, player.position.y - 100};

  // Update collectibles
  for (auto &coin : coins) {
    coin.Update(deltaTime);
    if (coin.CheckCollision(player.GetBounds())) {
      coin.collected = true;
      score += 10;
      // If it's the special key position, add key
      if (coin.position.x > 2100 && coin.position.x < 2300) {
        keys++;
      }
    }
  }

  // Update doors
  for (auto &door : doors) {
    door.Update(player, keys, doors[2]);
  }

  // Update enemies
  for (auto &enemy : enemies) {
    enemy.Update(deltaTime, player);
  }

  // Win condition (reach end door)
  if (doors.size() > 1 && doors[1].isOpen) {
    // Level complete!
    state = GAME_OVER;
  }

  // Pause
  if (IsKeyPressed(KEY_P))
    state = PAUSED;

  for (auto &layer : bgLayers) {
    layer.offsetX = -player.position.x * layer.speed;
  }
  for (auto &layer : fgLayers) {
    layer.offsetX = -player.position.x * layer.speed;
  }
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(SKYBLUE);

  for (auto &layer : bgLayers) {
    float x = fmod(layer.offsetX, layer.texture.width);
    DrawTexture(layer.texture, x, 0, WHITE);
    DrawTexture(layer.texture, x + layer.texture.width, 0, WHITE);
  }

  BeginMode2D(camera);
  // DrawRectangle(0, 0, 5000, SCREEN_HEIGHT, Color{135, 206, 235, 255});

  // Draw platforms from environment sprite
  for (const auto &plat : platforms) {
    // Tile the platform texture
    Rectangle source = {0, 64, GROUND_TILE_SIZE, GROUND_TILE_SIZE}; 
    for (float x = plat.x; x < plat.x + plat.width; x += GROUND_TILE_SIZE) {
      for (float y = plat.y; y < plat.y + plat.height; y += GROUND_TILE_SIZE) {
        DrawTexturePro(environmentSheet, source, {x, y, GROUND_TILE_SIZE, GROUND_TILE_SIZE}, {0, 0}, 0,
                       WHITE);
      }
    }
  }

  // Draw doors
  for (auto &door : doors)
    door.Draw(environmentSheet);

  // Draw collectibles
  for (auto &coin : coins)
    coin.Draw(environmentSheet);

  // Draw enemies
  for (auto &enemy : enemies)
    enemy.Draw(characterSheet);

  // Draw player
  player.Draw(characterSheet);

  // Draw attack hitbox debug (optional)
  // if (player.state == ATTACKING) {
  //   Rectangle atk = player.GetAttackHitbox();
  //   DrawRectangleRec(atk, ColorAlpha(RED, 0.3));
  // }

  EndMode2D();

  for (auto &layer : fgLayers) {
    float x = fmod(layer.offsetX, layer.texture.width);
    // Anchor bottom of foreground to world ground level so it stays down when player jumps - test
    int y = (int)((720.0f - camera.target.y) * camera.zoom + camera.offset.y) - layer.texture.height;
    DrawTexture(layer.texture, (int)x, y, WHITE);
    DrawTexture(layer.texture, (int)(x + layer.texture.width), y, WHITE);
  }

  // UI
  DrawText(TextFormat("SCORE: %d", score), 20, 20, 20, WHITE);
  DrawText(TextFormat("KEYS: %d", keys), 20, 50, 20, YELLOW);
  DrawText(TextFormat("HEALTH: %d", player.health), 20, 70, 20, WHITE);

  if (state == PAUSED) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.5));
    DrawText("PAUSED", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2, 40, WHITE);
    DrawText("Press P to resume", SCREEN_WIDTH / 2 - 100,
             SCREEN_HEIGHT / 2 + 50, 20, WHITE);
  }

  // Controls help
  DrawText("A/D: Move | SPACE: Jump | J: Attack | E: Interact", 20,
           SCREEN_HEIGHT - 30, 16, WHITE);

  EndDrawing();
}

void Game::HandleInput() {
  if (state == PAUSED && IsKeyPressed(KEY_P)) {
    state = PLAYING;
  }
}

Game::~Game() {
  UnloadTexture(characterSheet);
  UnloadTexture(environmentSheet);
  for (auto &layer : bgLayers)
    UnloadTexture(layer.texture);
  for (auto &layer : fgLayers)
    UnloadTexture(layer.texture);
  CloseWindow();
}

bool Game::ShouldClose() const { return WindowShouldClose(); }
