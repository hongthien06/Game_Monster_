#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <cmath>

// ===== VẼ THANH MÁU Ở TRÊN ĐẦU  =====
// ===== VẼ THANH MÁU Ở TRÊN ĐẦU  =====
inline void DrawHealthBar(SDL_Renderer* renderer, int currentHP, int maxHP,
    glm::vec2 position, glm::vec2 cameraOffset, float width, bool isPlayer = false) {
    if (currentHP <= 0 || maxHP <= 0) return;

    float healthPercent = (float)currentHP / (float)maxHP;

    // Vị trí thanh máu (phía trên đầu entity)
    float barX = position.x - cameraOffset.x - width / 2.0f + 24.0f; // Center thanh máu
    float barY = position.y - cameraOffset.y - 12.0f;
    float barWidth = width;
    float barHeight = 5.0f;

    // Vẽ background (đen với opacity)
    SDL_FRect bgRect = { barX, barY, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 200);
    SDL_RenderFillRect(renderer, &bgRect);

    // Chọn màu dựa vào % HP và loại entity
    SDL_Color barColor;

    if (isPlayer) {
        // PLAYER: Xanh lá → Vàng → Đỏ
        if (healthPercent > 0.6f) {
            barColor = { 46, 204, 113, 255 };     // Xanh lá đậm
        }
        else if (healthPercent > 0.3f) {
            barColor = { 241, 196, 15, 255 };     // Vàng
        }
        else {
            barColor = { 231, 76, 60, 255 };      // Đỏ
        }
    }
    else {
        // ENEMY: Đỏ → Vàng (khi yếu)
        if (healthPercent > 0.3f) {
            barColor = { 231, 76, 60, 255 };      // Đỏ
        }
        else {
            barColor = { 241, 196, 15, 255 };     // Vàng khi yếu
        }
    }

    // Vẽ thanh máu hiện tại với border radius giả lập
    SDL_FRect hpRect = { barX + 1, barY + 1, (barWidth - 2) * healthPercent, barHeight - 2 };
    SDL_SetRenderDrawColor(renderer, barColor.r, barColor.g, barColor.b, barColor.a);
    SDL_RenderFillRect(renderer, &hpRect);

    // Vẽ viền trắng mỏng
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 180);
    SDL_RenderRect(renderer, &bgRect);
}

// ===== VẼ THANH MÁU Ở GÓC
inline void DrawCornerHealthBar(SDL_Renderer* renderer, int currentHP, int maxHP,
    float x, float y, float width, float height, bool showHearts = false,
    SDL_Texture* heartTexture = nullptr) {
    if (maxHP <= 0) return;

    float healthPercent = (float)currentHP / (float)maxHP;
    healthPercent = (healthPercent < 0) ? 0 : (healthPercent > 1) ? 1 : healthPercent;


    SDL_FRect bgRect = { x, y, width, height };
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &bgRect);

    // ===== CHỌN MÀU DỰA VÀO % HP =====
    SDL_Color barColor;
    if (healthPercent > 0.6f) {
        barColor = { 46, 204, 113, 255 };      // Xanh lá
    }
    else if (healthPercent > 0.3f) {
        barColor = { 241, 196, 15, 255 };      // Vàng
    }
    else {
        barColor = { 231, 76, 60, 255 };       // Đỏ
    }

    // ===== VẼ THANH MÁU =====
    float currentWidth = (width - 4) * healthPercent;
    SDL_FRect hpRect = { x + 2, y + 2, currentWidth, height - 4 };
    SDL_SetRenderDrawColor(renderer, barColor.r, barColor.g, barColor.b, barColor.a);
    SDL_RenderFillRect(renderer, &hpRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    float segmentWidth = 20.0f; // Khoảng cách giữa các gạch
    int numSegments = (int)(width / segmentWidth);

    for (int i = 1; i < numSegments; i++) {
        float segX = x + i * segmentWidth;

        for (int j = 0; j < (int)height; j++) {
            float lineX = segX + (j * 6.0f / height);
            SDL_RenderPoint(renderer, lineX, y + j);
        }
    }

    // ===== VẼ VIỀN =====
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &bgRect);

   
}

// ===== KIỂM TRA VA CHẠM GIỮA 2 RECT =====
inline bool CheckCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return SDL_HasRectIntersectionFloat(&a, &b);
}

// ===== TÍNH KHOẢNG CÁCH GIỮA 2 ĐIỂM =====
inline float Distance(glm::vec2 a, glm::vec2 b) {
    return glm::length(b - a);
}

// ===== LERP (LINEAR INTERPOLATION) =====
inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline glm::vec2 Lerp(glm::vec2 a, glm::vec2 b, float t) {
    return glm::vec2(
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t)
    );
}


inline glm::vec2 GetDirection(glm::vec2 from, glm::vec2 to) {
    glm::vec2 dir = to - from;
    float len = glm::length(dir);
    if (len > 0) {
        return dir / len;
    }
    return glm::vec2(0.0f, 0.0f);
}

// ===== CLAMP VALUE =====
inline float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

inline int Clamp(int value, int min, int max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}