#include "HUD.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>

HUD::HUD(SDL_Renderer* renderer)
    : renderer(renderer), font(nullptr), coinIconTex(nullptr), score(0) {
}

HUD::~HUD() {
    Cleanup();
}

bool HUD::LoadResources() {
    font = TTF_OpenFont("assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 28);
    if (!font) {
        std::cerr << "Failed to load font: " << SDL_GetError() << std::endl;
        return false;
    }

    coinIconTex = IMG_LoadTexture(renderer, "assets/items/coin2_20x20.png");
    if (!coinIconTex) {
        std::cerr << "Failed to load coin icon: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void HUD::AddScore(int amount) {
    score += amount;
}

void HUD::AddScorePopup(glm::vec2 position, int value) {
    ScorePopup popup{ position, value, 1.5f, 255.0f };
    scorePopups.push_back(popup);
}

void HUD::Update(float deltaTime) {
    for (auto& popup : scorePopups) {
        popup.lifetime -= deltaTime;
        popup.position.y -= 30.0f * deltaTime;
        popup.alpha = (popup.lifetime / 1.5f) * 255.0f;
    }

    scorePopups.erase(
        std::remove_if(scorePopups.begin(), scorePopups.end(),
            [](const ScorePopup& p) { return p.lifetime <= 0; }),
        scorePopups.end());
}

void HUD::Render(glm::vec2 cameraOffset) {
    if (!font) return;

    // --- Vẽ popup điểm ---
    for (const auto& popup : scorePopups) {
        std::string text = "+" + std::to_string(popup.value);
        SDL_Color color = { 255, 255, 0, (Uint8)popup.alpha };

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 8, color);
        if (!surface) continue;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        float w, h;
        SDL_GetTextureSize(texture, &w, &h);

        SDL_FRect dstRect = {
            popup.position.x - cameraOffset.x - w / 2.0f,
            popup.position.y - cameraOffset.y - 30.0f,
            w, h
        };

        SDL_SetTextureAlphaModFloat(texture, popup.alpha / 255.0f);
        SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
        SDL_DestroyTexture(texture);
    }

    // --- Vẽ điểm tổng ---
    std::string scoreText = std::to_string(score) + "$";
    SDL_Color textColor = { 255, 235, 0, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(font, scoreText.c_str(), 8, textColor);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    float textWidth, textHeight;
    SDL_GetTextureSize(texture, &textWidth, &textHeight);

    SDL_FRect dstRect = { 40.0f, 13.0f, textWidth, textHeight };
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);

    if (coinIconTex) {
        SDL_FRect iconRect = { 12.0f, 20.0f, 20.0f, 20.0f };
        SDL_FRect iconSrc = { 0, 0, 20, 20 };
        SDL_RenderTexture(renderer, coinIconTex, &iconSrc, &iconRect);
    }
}

void HUD::Cleanup() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    if (coinIconTex) {
        SDL_DestroyTexture(coinIconTex);
        coinIconTex = nullptr;
    }
}
