#include "game.hpp"

Game::Game() : player(characterSheet), score(0), keys(0), state(PLAYING) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Side Scroller - Raylib");
  SetTargetFPS(60);

  // Load textures (adjust paths to your PNG files)
  characterSheet = LoadTexture("character_sprite.png");
  environmentSheet = LoadTexture("environment_sprite.png");

  bgLayers = {
      {LoadTexture("bg_05.png"), 0.1f, 0}, {LoadTexture("bg_04.png"), 0.3f, 0},
      {LoadTexture("bg_03.png"), 0.6f, 0}, {LoadTexture("bg_02.png"), 0.7f, 0},
      {LoadTexture("bg_01.png"), 0.8f, 0},
  };

  // Setup camera
  camera.target = {0, 0};
  camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f; // Zoom in for pixel art look

  // Create level
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
  platforms.push_back({500, 350, 100, 20});
  platforms.push_back({700, 250, 100, 20});

  // Collectibles
  coins.emplace_back(Vector2{350, 400}, environmentSheet);
  coins.emplace_back(Vector2{550, 300}, environmentSheet);
  coins.emplace_back(Vector2{750, 200}, environmentSheet);
  coins.emplace_back(Vector2{1000, 500}, environmentSheet);
  coins.emplace_back(Vector2{1500, 450}, environmentSheet);

  // Doors
  doors.emplace_back(750.0f, 536.0f, 0);  // No key needed
  doors.emplace_back(2800.0f, 286.0f, 1); // Needs 1 key

  // Enemies
  enemies.emplace_back(Enemy{1000, 502, 150});
  enemies.emplace_back(Enemy{1600, 452, 100});

  // Key pickup
  coins.emplace_back(Vector2{2200, 350},
                     environmentSheet); // This will be a key visually
}

void Game::Update(float deltaTime) {
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
    door.Update(player, keys);
  }

  // Update enemies
  for (auto &enemy : enemies) {
    enemy.Update(deltaTime, player);
  }

  // Win condition (reach end door)
  if (doors.size() > 1 && doors[1].isOpen &&
      CheckCollisionRecs(player.GetBounds(),
                         {doors[1].position.x, doors[1].position.y,
                          doors[1].width, doors[1].height})) {
    // Level complete!
  }

  // Pause
  if (IsKeyPressed(KEY_P))
    state = PAUSED;

  for (auto &layer : bgLayers) {
    layer.offsetX = -player.position.x * layer.speed;
  }
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(SKYBLUE);

  // Draw background (parallax could be added here)
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
    Rectangle source = {0, 64, 32, 32}; // Ground tile in env sprite
    for (float x = plat.x; x < plat.x + plat.width; x += 32) {
      for (float y = plat.y; y < plat.y + plat.height; y += 32) {
        DrawTexturePro(environmentSheet, source, {x, y, 32, 32}, {0, 0}, 0,
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
  if (player.state == ATTACKING) {
    Rectangle atk = player.GetAttackHitbox();
    DrawRectangleRec(atk, ColorAlpha(RED, 0.3));
  }

  EndMode2D();

  // UI
  DrawText(TextFormat("SCORE: %d", score), 20, 20, 20, WHITE);
  DrawText(TextFormat("KEYS: %d", keys), 20, 50, 20, YELLOW);

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
  CloseWindow();
}

bool Game::ShouldClose() const { return WindowShouldClose(); }