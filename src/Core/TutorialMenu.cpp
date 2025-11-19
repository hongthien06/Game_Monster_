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
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_FRect overlay = { 0.0f, 0.0f, (float)GameConstants::LOGICAL_WIDTH, (float)GameConstants::LOGICAL_HEIGHT };
    SDL_RenderFillRect(renderer, &overlay);

    float centerX = GameConstants::LOGICAL_WIDTH / 2.0f;
    float startY = 90.0f; 
    float lineHeight = 50.0f; 

    float columnCenterOffset = 90.0f; 

    float leftColumnX = centerX - columnCenterOffset;
    float rightColumnX = centerX + columnCenterOffset;

    float labelOffset = 60.0f; 

    SDL_Color colorTitle = { 255, 0, 0, 255 };
    SDL_Color colorLabel = { 255, 255, 255, 255 }; 
    SDL_Color colorKey = { 255, 255, 0, 255 };    
    SDL_Color colorButton = { 255, 255, 0, 255 }; 

    RenderText("TUTORIAL", { centerX, 50.0f }, colorTitle, true);

    float currentY = startY;
    RenderText("Move: A D", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText("Run: Shift", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText("Jump: Space", { leftColumnX, currentY }, colorLabel, true); currentY += lineHeight;

    currentY = startY; 
    RenderText("Attack: J", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText("Shot: K", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;
    RenderText("Dash: Ctrl", { rightColumnX, currentY }, colorLabel, true); currentY += lineHeight;

    SDL_FRect backRect = { centerX - 100.0f, GameConstants::LOGICAL_HEIGHT - 80.0f, 200.0f, 50.0f };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &backRect);
    RenderText("BACK", { centerX, GameConstants::LOGICAL_HEIGHT - 55.0f }, colorButton, true);
}
