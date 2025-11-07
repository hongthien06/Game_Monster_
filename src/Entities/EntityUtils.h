#pragma once
#include <SDL3/SDL.h>
#include <glm/glm.hpp>

// ===== VẼ THANH MÁU =====
inline void DrawHealthBar(SDL_Renderer* renderer, int currentHP, int maxHP,
    glm::vec2 position, glm::vec2 cameraOffset, float width) {
    if (currentHP <= 0 || maxHP <= 0) return;

    float healthPercent = (float)currentHP / (float)maxHP;

    // Vị trí thanh máu (phía trên đầu entity)
    float barX = position.x - cameraOffset.x;
    float barY = position.y - cameraOffset.y - 10.0f;
    float barWidth = width;
    float barHeight = 4.0f;

    // Vẽ background (đen)
    SDL_FRect bgRect = { barX, barY, barWidth, barHeight };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &bgRect);

    // Chọn màu dựa vào % HP
    SDL_Color barColor;
    if (healthPercent > 0.6f) {
        barColor = { 0, 255, 0, 255 };     // Xanh lá
    }
    else if (healthPercent > 0.3f) {
        barColor = { 255, 255, 0, 255 };   // Vàng
    }
    else {
        barColor = { 255, 0, 0, 255 };     // Đỏ
    }

    // Vẽ thanh máu hiện tại
    SDL_FRect hpRect = { barX, barY, barWidth * healthPercent, barHeight };
    SDL_SetRenderDrawColor(renderer, barColor.r, barColor.g, barColor.b, barColor.a);
    SDL_RenderFillRect(renderer, &hpRect);

    // Vẽ viền trắng
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

// ===== NORMALIZE DIRECTION =====
inline glm::vec2 GetDirection(glm::vec2 from, glm::vec2 to) {
    glm::vec2 dir = to - from;
    float len = glm::length(dir);
    if (len > 0) {
        return dir / len;
    }
    return glm::vec2(0.0f, 0.0f);
}

// ===== VẼ TEXT (DEBUG) =====
// Note: Cần SDL_ttf để vẽ text, đây chỉ là placeholder
inline void DrawDebugText(SDL_Renderer* renderer, const char* text,
    int x, int y, SDL_Color color) {
    // TODO: Implement với SDL_ttf
    // TTF_Font* font = ...
    // SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    // SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
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