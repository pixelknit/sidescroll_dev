#pragma once

struct ParallaxLayer {
    Texture2D texture;
    float speed;  // 0.0 = static, 0.5 = half speed, 1.0 = full speed
    float offsetX;
};