#include "raylib.h"
#include <vector>
#include <cmath>

// Game constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float GRAVITY = 800.0f;
const float JUMP_FORCE = -400.0f;
const float PLAYER_SPEED = 200.0f;
const float ANIMATION_SPEED = 10.0f;

// Game states
enum GameState { PLAYING, PAUSED, GAME_OVER };

// Animation states
enum PlayerState { IDLE, RUNNING, JUMPING, ATTACKING };

struct Animation {
    Rectangle frame;
    int frameCount;
    int currentFrame;
    float timer;
    
    Animation(Rectangle firstFrame, int count) 
        : frame(firstFrame), frameCount(count), currentFrame(0), timer(0) {}
    
    void Update(float deltaTime) {
        timer += deltaTime * ANIMATION_SPEED;
        if (timer >= 1.0f) {
            timer = 0;
            currentFrame = (currentFrame + 1) % frameCount;
        }
    }
    
    Rectangle GetCurrentFrame() const {
        return {
            frame.x + (frame.width * currentFrame),
            frame.y,
            frame.width,
            frame.height
        };
    }
};

struct Player {
    Vector2 position;
    Vector2 velocity;
    float width, height;
    bool facingRight;
    bool isGrounded;
    bool canAttack;
    float attackTimer;
    
    PlayerState state;
    Animation* currentAnim;
    Animation idleAnim;
    Animation runAnim;
    Animation jumpAnim;
    Animation attackAnim;
    
    Player(Texture2D& spriteSheet) 
        : position({100, 300}), velocity({0, 0}), 
          width(32), height(48), facingRight(true), 
          isGrounded(false), canAttack(true), attackTimer(0),
          state(IDLE),
          idleAnim({{0, 0, 32, 48}, 4}),      // Adjust based on your sprite
          runAnim({{0, 48, 32, 48}, 6}),      // Next row
          jumpAnim({{0, 96, 32, 48}, 2}),     // Jump frames
          attackAnim({{0, 144, 48, 48}, 4})   // Attack frames (wider)
    {
        currentAnim = &idleAnim;
    }
    
    void Update(float deltaTime, const std::vector<Rectangle>& platforms) {
        // Input handling
        velocity.x = 0;
        
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
            velocity.x = -PLAYER_SPEED;
            facingRight = false;
            if (isGrounded && state != ATTACKING) state = RUNNING;
        }
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
            velocity.x = PLAYER_SPEED;
            facingRight = true;
            if (isGrounded && state != ATTACKING) state = RUNNING;
        }
        
        // Jump
        if ((IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W)) && isGrounded && state != ATTACKING) {
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
        
        for (const auto& platform : platforms) {
            if (CheckCollisionRecs(playerRect, platform)) {
                // Landing on top
                if (velocity.y > 0 && playerRect.y + playerRect.height - velocity.y * deltaTime <= platform.y) {
                    position.y = platform.y - height;
                    velocity.y = 0;
                    isGrounded = true;
                    if (state == JUMPING) state = IDLE;
                }
                // Hitting head
                else if (velocity.y < 0 && playerRect.y - velocity.y * deltaTime >= platform.y + platform.height) {
                    position.y = platform.y + platform.height;
                    velocity.y = 0;
                }
                // Side collisions
                else if (velocity.x > 0) {
                    position.x = platform.x - width;
                }
                else if (velocity.x < 0) {
                    position.x = platform.x + platform.width;
                }
            }
        }
        
        // Screen boundaries
        if (position.x < 0) position.x = 0;
        if (position.x > 5000) position.x = 5000; // Level width limit
        
        // Attack timer
        if (!canAttack) {
            attackTimer -= deltaTime;
            if (attackTimer <= 0) {
                canAttack = true;
                if (isGrounded) state = IDLE;
                else state = JUMPING;
            }
        }
        
        // Set animation based on state
        switch(state) {
            case IDLE: currentAnim = &idleAnim; break;
            case RUNNING: currentAnim = &runAnim; break;
            case JUMPING: currentAnim = &jumpAnim; break;
            case ATTACKING: currentAnim = &attackAnim; break;
        }
        
        currentAnim->Update(deltaTime);
    }
    
    void Draw(Texture2D& spriteSheet) {
        Rectangle source = currentAnim->GetCurrentFrame();
        
        // Flip if facing left
        if (!facingRight) source.width = -source.width;
        
        Rectangle dest = {position.x, position.y, width, height};
        Vector2 origin = {0, 0};
        
        DrawTexturePro(spriteSheet, source, dest, origin, 0.0f, WHITE);
        
        // Debug hitbox (remove in final)
        // DrawRectangleLines(position.x, position.y, width, height, RED);
    }
    
    Rectangle GetBounds() const {
        return {position.x, position.y, width, height};
    }
    
    Rectangle GetAttackHitbox() const {
        if (state != ATTACKING) return {0, 0, 0, 0};
        
        float attackRange = 30;
        if (facingRight) {
            return {position.x + width, position.y + 10, attackRange, height - 20};
        } else {
            return {position.x - attackRange, position.y + 10, attackRange, height - 20};
        }
    }
};

struct Collectible {
    Vector2 position;
    float radius;
    bool collected;
    Animation anim;
    
    Collectible(Vector2 pos, Texture2D sheet) 
        : position(pos), radius(12), collected(false),
          anim({{0, 200, 16, 16}, 8}) {} // Adjust frame in your sprite
    
    void Update(float deltaTime) {
        if (!collected) anim.Update(deltaTime);
    }
    
    void Draw(Texture2D& spriteSheet) {
        if (collected) return;
        
        Rectangle source = anim.GetCurrentFrame();
        Rectangle dest = {position.x - radius, position.y - radius, radius*2, radius*2};
        DrawTexturePro(spriteSheet, source, dest, {0,0}, 0, WHITE);
    }
    
    bool CheckCollision(const Rectangle& player) {
        if (collected) return false;
        Rectangle itemRect = {position.x - radius, position.y - radius, radius*2, radius*2};
        return CheckCollisionRecs(player, itemRect);
    }
};

struct Door {
    Vector2 position;
    float width, height;
    bool isOpen;
    bool isLocked;
    int requiredKeys;
    
    Door(float x, float y, int keysNeeded = 0)
        : position({x, y}), width(48), height(64), 
          isOpen(false), isLocked(keysNeeded > 0), requiredKeys(keysNeeded) {}
    
    void Update(Player& player, int playerKeys) {
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
    
    void Draw(Texture2D& envSheet) {
        // Door frames in environment sprite (adjust coordinates)
        Rectangle source;
        if (isOpen) {
            source = {96, 0, 48, 64}; // Open door frame
        } else if (isLocked) {
            source = {48, 0, 48, 64}; // Locked door frame
        } else {
            source = {0, 0, 48, 64};  // Closed door frame
        }
        
        Rectangle dest = {position.x, position.y, width, height};
        DrawTexturePro(envSheet, source, dest, {0,0}, 0, WHITE);
        
        // Interaction prompt
        if (!isOpen) {
            DrawText("Press E", position.x - 10, position.y - 20, 10, WHITE);
        }
    }
};

struct Enemy {
    Vector2 position;
    Vector2 startPos;
    float patrolDistance;
    float speed;
    float width, height;
    bool alive;
    int health;
    
    Enemy(float x, float y, float patrolDist = 100)
        : position({x, y}), startPos({x, y}), patrolDistance(patrolDist),
          speed(50), width(32), height(48), alive(true), health(2) {}
    
    void Update(float deltaTime, Player& player) {
        if (!alive) return;
        
        // Simple patrol AI
        float offset = sin(GetTime() * speed * 0.01) * patrolDistance;
        position.x = startPos.x + offset;
        
        // Check if hit by player attack
        if (player.state == ATTACKING && CheckCollisionRecs(GetBounds(), player.GetAttackHitbox())) {
            health--;
            if (health <= 0) alive = false;
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
    
    void Draw(Texture2D& spriteSheet) {
        if (!alive) return;
        
        // Use enemy frames from character sheet or separate enemy sprite
        Rectangle source = {0, 192, 32, 48}; // Adjust based on your sprite
        Rectangle dest = {position.x, position.y, width, height};
        DrawTexturePro(spriteSheet, source, dest, {0,0}, 0, WHITE);
    }
    
    Rectangle GetBounds() const {
        return {position.x, position.y, width, height};
    }
};

struct ParallaxLayer {
    Texture2D texture;
    float speed;  // 0.0 = static, 0.5 = half speed, 1.0 = full speed
    float offsetX;
};

struct Game {
    Texture2D characterSheet;
    Texture2D environmentSheet;
    
    Player player;
    std::vector<Rectangle> platforms;
    std::vector<Collectible> coins;
    std::vector<Door> doors;
    std::vector<Enemy> enemies;
    
    Camera2D camera;
    int score;
    int keys;
    GameState state;

    std::vector<ParallaxLayer> bgLayers;
    
    Game() : player(characterSheet), score(0), keys(0), state(PLAYING) {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Side Scroller - Raylib");
        SetTargetFPS(60);
        
        // Load textures (adjust paths to your PNG files)
        characterSheet =    LoadTexture("character_sprite.png");
        environmentSheet =  LoadTexture("environment_sprite.png");
        
        bgLayers = {
            {LoadTexture("bg.png"), 0.1f, 0},
            {LoadTexture("bg.png"), 0.3f, 0},
            {LoadTexture("bg.png"), 0.6f, 0}
        };

        // Setup camera
        camera.target = {0, 0};
        camera.offset = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
        camera.rotation = 0.0f;
        camera.zoom = 2.0f; // Zoom in for pixel art look
        
        // Create level
        CreateLevel();
    }
    

    void CreateLevel() {
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
        doors.emplace_back(750.0f, 536.0f, 0);      // No key needed
        doors.emplace_back(2800.0f, 286.0f, 1);     // Needs 1 key
        
        // Enemies
        enemies.emplace_back(Enemy{1000, 502, 150});
        enemies.emplace_back(Enemy{1600, 452, 100});
        
        // Key pickup
        coins.emplace_back(Vector2{2200, 350}, environmentSheet); // This will be a key visually
    }
    
    void Update(float deltaTime) {
        if (state != PLAYING) return;
        
        // Update player
        player.Update(deltaTime, platforms);

        //Update BG
        // UpdateParallax(player.position.x);
        
        // Update camera to follow player
        camera.target = {
            player.position.x,
            player.position.y - 100
        };
        
        // Update collectibles
        for (auto& coin : coins) {
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
        for (auto& door : doors) {
            door.Update(player, keys);
        }
        
        // Update enemies
        for (auto& enemy : enemies) {
            enemy.Update(deltaTime, player);
        }
        
        // Win condition (reach end door)
        if (doors.size() > 1 && doors[1].isOpen && CheckCollisionRecs(player.GetBounds(), 
            {doors[1].position.x, doors[1].position.y, doors[1].width, doors[1].height})) {
            // Level complete!
        }
        
        // Pause
        if (IsKeyPressed(KEY_P)) state = PAUSED;

        for (auto& layer : bgLayers) {
            layer.offsetX = -player.position.x * layer.speed;
        }
    }
    
    void Draw() {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Draw background (parallax could be added here)
        for (auto& layer : bgLayers) {
            float x = fmod(layer.offsetX, layer.texture.width);
            DrawTexture(layer.texture, x, 0, WHITE);
            DrawTexture(layer.texture, x + layer.texture.width, 0, WHITE);
        }
        
        BeginMode2D(camera);
        // DrawRectangle(0, 0, 5000, SCREEN_HEIGHT, Color{135, 206, 235, 255});
        
        // Draw platforms from environment sprite
        for (const auto& plat : platforms) {
            // Tile the platform texture
            Rectangle source = {0, 64, 32, 32}; // Ground tile in env sprite
            for (float x = plat.x; x < plat.x + plat.width; x += 32) {
                for (float y = plat.y; y < plat.y + plat.height; y += 32) {
                    DrawTexturePro(environmentSheet, source, 
                        {x, y, 32, 32}, {0,0}, 0, WHITE);
                }
            }
        }
        
        // Draw doors
        for (auto& door : doors) door.Draw(environmentSheet);
        
        // Draw collectibles
        for (auto& coin : coins) coin.Draw(environmentSheet);
        
        // Draw enemies
        for (auto& enemy : enemies) enemy.Draw(characterSheet);
        
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
            DrawText("PAUSED", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2, 40, WHITE);
            DrawText("Press P to resume", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 20, WHITE);
        }
        
        // Controls help
        DrawText("A/D: Move | SPACE: Jump | J: Attack | E: Interact", 20, SCREEN_HEIGHT - 30, 16, WHITE);
        
        EndDrawing();
    }
    
    void HandleInput() {
        if (state == PAUSED && IsKeyPressed(KEY_P)) {
            state = PLAYING;
        }
    }
    
    ~Game() {
        UnloadTexture(characterSheet);
        UnloadTexture(environmentSheet);
        for (auto& layer : bgLayers) UnloadTexture(layer.texture);
        CloseWindow();
    }
    
    bool ShouldClose() const {
        return WindowShouldClose();
    }
};

int main() {
    Game game;
    
    while (!game.ShouldClose()) {
        float deltaTime = GetFrameTime();
        
        game.HandleInput();
        game.Update(deltaTime);
        game.Draw();
    }
    
    return 0;
}