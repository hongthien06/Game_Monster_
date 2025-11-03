
#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

// Hàm vẽ thanh máu trên đầu
inline void DrawHealthBar(
    SDL_Renderer* renderer,
    int currentHealth,
    int maxHealth,
    glm::vec2 position,
    glm::vec2 cameraOffset,
    float spriteWidth)
{
    // Chỉ vẽ khi máu không đầy và còn máu
    if (currentHealth <= maxHealth && currentHealth > 0) {
        float healthPercent = (float)currentHealth / (float)maxHealth;

        // Kích thước thanh máu
        const float healthBarWidth = spriteWidth * 0.8; // Chiếm 75% chiều rộng sprite
        const float healthBarHeight = 5.0f;
        const float verticalOffset = -10.0f; // 0 pixel trên đầu

        // Vị trí (căn giữa sprite)
        const float xOffset = (spriteWidth - healthBarWidth) * 1.5 ;

        // Nền (màu đỏ)
        SDL_FRect bgRect = {
            position.x - cameraOffset.x + xOffset,
            position.y - cameraOffset.y - verticalOffset,
            healthBarWidth,
            healthBarHeight
        };
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Đỏ
        SDL_RenderFillRect(renderer, &bgRect);

        // Máu hiện tại (màu xanh lá)
        SDL_FRect fgRect = bgRect;
        fgRect.w = healthBarWidth * healthPercent;
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Xanh lá
        SDL_RenderFillRect(renderer, &fgRect);

        // Quan trọng: Đặt màu vẽ về màu mặc định sau khi dùng
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    }
}