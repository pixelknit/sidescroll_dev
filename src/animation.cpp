#include "animation.hpp"
#include "config.h"

Animation::Animation(Rectangle firstFrame, int count)
  : frame(firstFrame), frameCount(count), currentFrame(0), timer(0) {}

void Animation::Update(float deltaTime) {
  timer += deltaTime * ANIMATION_SPEED;
  if (timer >= 1.0f) {
    timer = 0;
    currentFrame = (currentFrame + 1) % frameCount;
  }
}

Rectangle Animation::GetCurrentFrame() const {
  return {frame.x + (frame.width * currentFrame), frame.y, frame.width,
          frame.height};
}