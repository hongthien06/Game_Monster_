#include "TutorialMenu.h"
#include "../Config/GameConstants.h"
#include <iostream>

TutorialMenu::TutorialMenu(SDL_Renderer* ren, TTF_Font* fnt)
    : renderer(ren), font(fnt) {
    Reset();
}

TutorialMenu::~TutorialMenu() {}

void TutorialMenu::Reset() {
    selectedOption = 0;
    inputTimer = inputCooldown;
    currentChoice = TutorialChoice::NONE;
}

void TutorialMenu::Update(float deltaTime) {
    if (inputTimer > 0) inputTimer -= deltaTime;
}

void TutorialMenu::HandleInput() {
    if (inputTimer > 0) return;

    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_ESCAPE] || keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_J]) {
        currentChoice = TutorialChoice::BACK;
        inputTimer = inputCooldown;
    }
}

void TutorialMenu::HandleMouseClick(float mouseX, float mouseY) {
    float buttonX = GameConstants::LOGICAL_WIDTH / 2.0f - 100.0f;
    float buttonY = GameConstants::LOGICAL_HEIGHT - 100.0f;
    float buttonW = 200.0f;
    float buttonH = 50.0f;

    if (mouseX >= buttonX && mouseX <= buttonX + buttonW &&
        mouseY >= buttonY && mouseY <= buttonY + buttonH) {
        currentChoice = TutorialChoice::BACK;
    }
}

void TutorialMenu::RenderText(const std::string& text, glm::vec2 pos, SDL_Color color, bool center) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(),0, color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface); // SDL3 dùng DestroySurface
    if (!texture) return;

    SDL_FRect dstRect;
    dstRect.w = (float)surface->w;
    dstRect.h = (float)surface->h;
    if (center) {
        dstRect.x = pos.x - dstRect.w / 2.0f;
        dstRect.y = pos.y - dstRect.h / 2.0f;
    }
    else {
        dstRect.x = pos.x;
        dstRect.y = pos.y;
    }

    SDL_RenderTexture(renderer, texture, nullptr, &dstRect);
    SDL_DestroyTexture(texture);
}

void TutorialMenu::Render() {
    // Nền overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlay = { 0.0f, 0.0f, (float)GameConstants::SCREEN_WIDTH, (float)GameConstants::SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;

    SDL_Color colorTitle = { 0, 255, 0, 255 };
    SDL_Color colorText = { 255, 255, 255, 255 };
    SDL_Color colorButton = { 255, 255, 0, 255 };

    // Hiển thị hướng dẫn
    RenderText("TUTORIAL", { centerX, 50.0f }, colorTitle, true);
    RenderText("Use W/S or UP/DOWN to move the character.", { 50, 150.0f }, colorText);
    RenderText("Press J or ENTER to attack.", { 50, 200.0f }, colorText);
    RenderText("Press ESC, ENTER, or click BACK to return.", { 50, 250.0f }, colorText);

    // Vẽ BACK button
    SDL_FRect backRect = { centerX - 100.0f, GameConstants::LOGICAL_HEIGHT - 100.0f, 200.0f, 50.0f };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &backRect);

    RenderText("BACK", { centerX, GameConstants::LOGICAL_HEIGHT - 75.0f }, colorButton, true);
}
