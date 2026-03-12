#pragma once

#include "raylib.h"

struct Animation {
  Rectangle frame;
  int frameCount;
  int currentFrame;
  float timer;

  Animation(Rectangle firstFrame, int count);
  void Update(float deltaTime);
  Rectangle GetCurrentFrame() const;
};