#pragma once

#include <vector>

inline constexpr int SCREEN_WIDTH = 1280;
inline constexpr int SCREEN_HEIGHT = 720;
inline constexpr int PLAYER_SIZE = 170;
inline constexpr float GRAVITY = 800.0f;
inline constexpr float JUMP_FORCE = -400.0f;
inline constexpr float PLAYER_SPEED = 200.0f;
inline constexpr float ANIMATION_SPEED = 10.0f;

//constexpr std::vector can be used on c++ 20 onwards
inline std::vector<float> BG_LAYER_PARALLAX_MOTION_SPEED = {
   0.1, 0.3, 0.6, 0.7, 0.8 
};

// Game states
enum GameState { PLAYING, PAUSED, GAME_OVER };

// Animation states
enum PlayerState { IDLE, RUNNING, JUMPING, ATTACKING };