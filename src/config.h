#pragma once

inline constexpr int SCREEN_WIDTH = 1280;
inline constexpr int SCREEN_HEIGHT = 720;
inline constexpr float GRAVITY = 800.0f;
inline constexpr float JUMP_FORCE = -400.0f;
inline constexpr float PLAYER_SPEED = 200.0f;
inline constexpr float ANIMATION_SPEED = 10.0f;

// Game states
enum GameState { PLAYING, PAUSED, GAME_OVER };

// Animation states
enum PlayerState { IDLE, RUNNING, JUMPING, ATTACKING };