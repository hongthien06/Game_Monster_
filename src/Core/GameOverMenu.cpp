#include "GameOverMenu.h"
#include <iostream>
#include <glm/glm.hpp> 
#include "../Config/GameConstants.h"

void RenderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text,
    glm::vec2 pos, SDL_Color color, bool center = false)
{
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dstRect;
    dstRect.x = pos.x;
    dstRect.y = pos.y;
    dstRect.w = (float)surface->w;
    dstRect.h = (float)surface->h;

    if (center) {
        dstRect.x = pos.x - dstRect.w / 2.0f;
        dstRect.y = pos.y - dstRect.h / 2.0f;
    }

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


GameOverMenu::GameOverMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {

    Reset();
}

GameOverMenu::~GameOverMenu() {
}

void GameOverMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = MenuChoice::NONE;
}

void GameOverMenu::Update(float deltaTime) {
    if (inputTimer > 0) {
        inputTimer -= deltaTime;
    }
}

void GameOverMenu::HandleInput() {
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) {
        if (selectedOption > 0) {
            selectedOption = 0;
            inputTimer = inputCooldown;
        }
    }
    else if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) {
        if (selectedOption < 1) {
            selectedOption = 1;
            inputTimer = inputCooldown;
        }
    }
    else if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        if (selectedOption == 0) {
            currentChoice = MenuChoice::REPLAY;
        }
        else if (selectedOption == 1) {
            currentChoice = MenuChoice::QUIT;
        }
        inputTimer = inputCooldown;
    }
}

void GameOverMenu::Render() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlayRect = { 0.0f, 0.0f, (float)GameConstants::SCREEN_WIDTH, (float)GameConstants::SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlayRect);

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float centerY = GameConstants::LOGICAL_HEIGHT / 2.0f;

    SDL_Color colorTitle = { 255, 0, 0, 255 };
    SDL_Color colorReplay = (selectedOption == 0) ? SDL_Color{ 255, 255, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };
    SDL_Color colorQuit = (selectedOption == 1) ? SDL_Color{ 255, 255, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };

    RenderText(renderer, font, "GAME OVER", { centerX, centerY - 50.0f }, colorTitle, true);
    RenderText(renderer, font, "Quay Lai", { centerX, centerY + 10.0f }, colorReplay, true);
    RenderText(renderer, font, "Thoat Game", { centerX, centerY + 40.0f }, colorQuit, true);
}