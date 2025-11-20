#include "HUD.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>
#include "../Entities/Player.h"

HUD::HUD(SDL_Renderer* renderer)
    : renderer(renderer),
    font(nullptr),
    coinIconTex(nullptr),
    potionIconTex(nullptr),
    score(0),
    playerRef(nullptr)
{
}

HUD::~HUD() {
    Cleanup();
}

bool HUD::LoadResources() {
    font = TTF_OpenFont("assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 16);
    if (!font) {
        std::cerr << "Failed to load font: " << SDL_GetError() << std::endl;
        return false;
    }

    coinIconTex = IMG_LoadTexture(renderer, "assets/items/coin2_20x20.png");
    if (!coinIconTex) {
        std::cerr << "Failed to load coin icon: " << SDL_GetError() << std::endl;
        return false;
    }

    
    potionIconTex = IMG_LoadTexture(renderer, "assets/items/potion4.png");
    if (!potionIconTex) {
        std::cerr << "ERROR: Failed to load potion icon: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "[HUD] Potion icon loaded successfully!" << std::endl;

    return true;
}

//hàm cộng điểm coin
void HUD::AddScore(int amount) {
    score += amount;
}

//hàm hiện popup số coin khi nhặt
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

//hàm vẽ tất cả trên màn hình 
void HUD::Render(glm::vec2 cameraOffset) {
    if (!font) return;

    // --- Vẽ popup điểm ---
    for (const auto& popup : scorePopups) {
        std::string text = "+" + std::to_string(popup.value);
        SDL_Color color = { 255, 255, 0, (Uint8)popup.alpha };

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 4, color);
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

    // --- Vẽ điểm tổng (Coin) ---
    std::string scoreText = std::to_string(score) + "$";
    SDL_Color textColor = { 255, 235, 0, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(font, scoreText.c_str(), 4, textColor);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    float textWidth, textHeight;
    SDL_GetTextureSize(texture, &textWidth, &textHeight);

    SDL_FRect dstRect = { 36.0f, 16.0f, textWidth, textHeight };
    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);

    // Vẽ coin icon
    if (coinIconTex) {
        SDL_FRect iconRect = { 9.0f, 13.0f, 20.0f, 20.0f };
        SDL_FRect iconSrc = { 0, 0, 18, 18 };
        SDL_RenderTexture(renderer, coinIconTex, &iconSrc, &iconRect);
    }

    // ===== DEBUG: VẼ SỐ LƯỢNG HEALTH POTION =====
    // Kiểm tra từng điều kiện
    if (!playerRef) {
        std::cerr << "[HUD] ERROR: playerRef is NULL!" << std::endl;
        return;
    }

    if (!potionIconTex) {
        std::cerr << "[HUD] ERROR: potionIconTex is NULL!" << std::endl;
        return;
    }

    
    int potionCount = playerRef->GetHealthPotionCount();
    int maxPotions = playerRef->GetMaxHealthPotions();

    // Vẽ icon potion
    SDL_FRect potionIconRect = { 70.0f, 13.0f, 24.0f, 24.0f };
    SDL_FRect potionIconSrc = { 0, 0, 64, 64 };
    SDL_RenderTexture(renderer, potionIconTex, &potionIconSrc, &potionIconRect);

    // Vẽ text số lượng
    std::string potionText = std::to_string(potionCount) + "/" + std::to_string(maxPotions);
    SDL_Color potionColor = { 0, 255, 100, 255 };

    SDL_Surface* potionSurface = TTF_RenderText_Blended(font, potionText.c_str(), 4, potionColor);
    if (potionSurface) {
        SDL_Texture* potionTexture = SDL_CreateTextureFromSurface(renderer, potionSurface);
        SDL_DestroySurface(potionSurface);

        float potionTextW, potionTextH;
        SDL_GetTextureSize(potionTexture, &potionTextW, &potionTextH);

        SDL_FRect potionTextRect = { 95.0f, 17.0f, potionTextW, potionTextH };
        SDL_RenderTexture(renderer, potionTexture, nullptr, &potionTextRect);
        SDL_DestroyTexture(potionTexture);
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
    if (potionIconTex) {
        SDL_DestroyTexture(potionIconTex);
        potionIconTex = nullptr;
    }
}
void HUD::Reset() {
    score = 0;
    scorePopups.clear();
}